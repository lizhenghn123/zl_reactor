#include <iostream>
#include <assert.h>
#include "base/Logger.h"
#include "base/Timestamp.h"
#include "net/TcpServer.h"
#include "net/TcpConnection.h"
#include "net/EventLoop.h"
#include "net/NetUtil.h"
using namespace std;
using namespace zl::net;
using namespace zl::base;

// 利用reactor模式实现的几个应用层的简单协议
// see http://blog.csdn.net/solstice/article/details/6171905

/// 陈硕总结的关于 TCP 网络编程最本质的是处理三个半事件： http://blog.csdn.net/Solstice/article/details/6171831#essential
/// 1. 连接的建立，包括服务端接受 (accept) 新连接和客户端成功发起 (connect) 连接。
/// 2. 连接的断开，包括主动断开 (close 或 shutdown) 和被动断开 (read 返回 0)。
/// 3. 消息到达，文件描述符可读。这是最为重要的一个事件，对它的处理方式决定了网络编程的风格（阻塞还是非阻塞，如何处理分包，应用层的缓冲如何设计等等）。
/// 4. 消息发送完毕，这算半个。对于低流量的服务，可以不必关心这个事件；
///         另外，这里“发送完毕”是指将数据写入操作系统的缓冲区，将由 TCP 协议栈负责数据的发送与重传，不代表对方已经收到数据。

// test client  : nc 127.0.0.1 8888 | hexdump -C
template <class ProtocolServer>
class DemoServer
{
public:
    DemoServer(EventLoop *loop, const InetAddress& listenAddr, const string& serverName)
        : server_(loop, listenAddr, serverName)
    {
        server_.setConnectionCallback(std::bind(&DemoServer::onConnection, this, std::placeholders::_1));
        
        server_.setMessageCallback(std::bind(&ProtocolServer::onMessage, &protocolPolicy, 
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        server_.setWriteCompleteCallback(std::bind(&ProtocolServer::onWriteComplete, &protocolPolicy, std::placeholders::_1));

        printf("start server[%s] \n", serverName.c_str());
    }

    void start()
    { 
        //server_.setThreadNum(4);
        server_.start();
    }

    void stop()
    { 
        //server_.stop();
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        cout  << "DemoServer::onConnection " << conn->peerAddress().ipPort() << " is "
            << (conn->connected() ? "UP" : "DOWN") << "\n";
        protocolPolicy.onConnection(conn);
    }

private:
    TcpServer server_;
    ProtocolServer protocolPolicy;
};

/// Discard 是最简单的长连接TCP应用层协议，服务端收到数据后直接丢弃
/// 只需要关注“三个半事件”中的“消息/数据到达”事件
struct DiscardProtocolServer// : public DemoServer
{
    void onConnection(const TcpConnectionPtr& conn)
    {
    }

    void onMessage(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp time)
    {
        string msg(buf->retrieveAllAsString());
        cout << "DiscardServer::onMessage, fd [" << conn->fd() << "], "<< msg.size() << " bytes, ["
            << msg.data() << "], received at " << time.toString() << ", ";
        cout << "just discard data\n";
    }

    void onWriteComplete(const TcpConnectionPtr& conn)
    {

    }
};

/// Daytime 是短连接协议，在发送完当前时间后，由服务端主动断开连接
/// 它只需要关注“三个半事件”中的“连接已建立”事件
struct DayTimeProtocolServer
{
    void onConnection(const TcpConnectionPtr& conn)
    {
        // TODO , 另外注意：禁用之前client就可能已经发送数据了，是否需要读取出内核中保存的数据以释放内存
        conn->disableReading();  // 取消读事件监听

        Timestamp now(Timestamp::now());
        conn->send(now.toString() + "\n"); // 发送服务器当前时间
        conn->shutdown(); // 主动断开连接
    }

    void onMessage(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp time)
    {
        assert(0 && "daytime protocol do not need receive data from client" && "DayTimeProtocolServer::onMessage");
    }

    void onWriteComplete(const TcpConnectionPtr& conn)
    {

    }
};

/// Time 协议与 daytime 类似，只不过它返回的不是日期时间字符串，而是一个 32-bit 整数，
/// 表示从 1970-01-01 00:00:00 到现在的秒数
/// 它只需要关注“三个半事件”中的“连接已建立”事件
struct TimeProtocolServer
{
    void onConnection(const TcpConnectionPtr& conn)
    {
        // TODO , 另外注意：禁用之前client就可能已经发送数据了，是否需要读取出内核中保存的数据以释放内存
        conn->disableReading();  // 取消读事件监听

        time_t now = time(NULL);
        int time = static_cast<int>(now);
        int time2 = NetUtil::host2Net<int32_t>(time);

        printf("time : %d : %d\n", time, time2);
        conn->send((const void *)&time2, sizeof(time2)); // 发送服务器当前时间
        conn->shutdown(); // 主动断开连接
    }

    void onMessage(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp time)
    {
        assert(0 && "time protocol do not need receive data from client" && "TimeProtocolServer::onMessage");
    }

    void onWriteComplete(const TcpConnectionPtr& conn)
    {

    }
};

/// Echo协议 是一个简单的带交互的协议 : 服务端把客户端发过来的数据原封不动地传回去
/// 它只需要关注“三个半事件”中的“消息/数据到达”事件
struct EchoProtocolServer
{
    void onConnection(const TcpConnectionPtr& conn)
    {
    }

    void onMessage(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp time)
    {
        string msg(buf->retrieveAllAsString());
        cout << "EchoServer::onMessage, fd [" << conn->fd() << "], "<< msg.size() << " bytes, ["
            << msg.data() << "], received at " << time.toString() << "\n";
        conn->send(msg.data(), msg.size());
    }

    void onWriteComplete(const TcpConnectionPtr& conn)
    {

    }
};
/// Chargen 协议很特殊，它只发送数据，不接收数据,而且，它发送数据的速度不能快过客户端接收的速度
/// 需要关注“三个半事件”中的半个“消息/数据发送完毕”事件(onWriteComplete)
struct ChargenProtocolServer
{
     ChargenProtocolServer()
    {
        string line;
        for (int i = 33; i < 127; ++i)
        {
            line.push_back(char(i));
        }
        line += line;

        for (size_t i = 0; i < 127-33; ++i)
        {
            message_ += line.substr(i, 72) + '\n';
        }
    }

    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected())
        {
            int fd = conn->fd();
            SocketUtil::setNoDelay(fd, true);
            conn->send(message_);
        }
    }

    void onMessage(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp time)
    {
        string msg(buf->retrieveAllAsString());
        cout << "EchoServer::onMessage, fd [" << conn->fd() << "], "<< msg.size() << " bytes, ["
            << msg.data() << "], received at " << time.toString() << "\n";
        conn->send(msg.data(), msg.size());
    }

    void onWriteComplete(const TcpConnectionPtr& conn)
    {
        transferred_ += message_.size();
        conn->send(message_);
        printThroughput();
    }

    void printThroughput()
    {
        Timestamp endTime = Timestamp::now();
        double time = Timestamp::timeDiff(endTime, startTime_);
        printf("%4.3f MiB/s\n", static_cast<double>(transferred_)/time/1024/1024);
        transferred_ = 0;
        startTime_ = endTime;
    }

    string message_;
    int64_t transferred_;
    Timestamp startTime_;
};

int main()
{
    int printUsageAndGetChoice();

    int flag = printUsageAndGetChoice();

    EventLoop loop;
    InetAddress listenAddr("127.0.0.1", 8888);

    if(flag == 1)
    {
        DemoServer<DiscardProtocolServer> server(&loop, listenAddr, "DiscardServer");
        server.start();
        loop.loop();
    }
    else if(flag == 2)
    {
        DemoServer<DayTimeProtocolServer> server(&loop, listenAddr, "DaytimeServer");
        server.start();
        loop.loop();
    }
    else if(flag == 3)
    {
        DemoServer<TimeProtocolServer> server(&loop, listenAddr, "TimeServer");
        server.start();
        loop.loop();
    }
    else if(flag == 4)
    {
        DemoServer<EchoProtocolServer> server(&loop, listenAddr, "EchoServer");
        server.start();
        loop.loop();
    }
    else if(flag == 5)
    {
        DemoServer<ChargenProtocolServer> server(&loop, listenAddr, "ChargenServer");
        server.start();
        loop.loop();
    }
    else if(flag == 6)
    {
        InetAddress listenAddr1("127.0.0.1", 9001);
        DemoServer<DiscardProtocolServer> server1(&loop, listenAddr1, "DiscardServer");

        InetAddress listenAddr2("127.0.0.1", 9002);
        DemoServer<DayTimeProtocolServer> server2(&loop, listenAddr2, "DaytimeServer");

        InetAddress listenAddr3("127.0.0.1", 9003);
        DemoServer<TimeProtocolServer> server3(&loop, listenAddr3, "TimeServer");

        InetAddress listenAddr4("127.0.0.1", 9004);
        DemoServer<EchoProtocolServer> server4(&loop, listenAddr4, "EchoServer");

        InetAddress listenAddr5("127.0.0.1", 9005);
        DemoServer<ChargenProtocolServer> server5(&loop, listenAddr5, "ChargenServer");

        server1.start();
        server2.start();
        server3.start();
        server4.start();
        server5.start();

        loop.loop();
    }
    return 0;
}

int printUsageAndGetChoice()
{
    printf("-------------- Test discard、daytime、time、echo protocol server --------------\n");
    printf("input 1 : test discard protocol, TCP port 8888\n");
    printf("input 2 : test daytime protocol, TCP port 8888\n");
    printf("input 3 : test time protocol, TCP port 8888\n");
    printf("input 4 : test echo protocol, TCP port 8888\n");
    printf("input 5 : test chargen protocol, TCP port 8888\n");
    printf("input 6 : test above all, TCP port 9001,  9002,9003, 9004, 9005\n");
    int i = 0;
    while(true)
    {
        printf("now input your choice : ");
        scanf("%d", &i);
        if(i > 0 && i <= 6)
            return i;
        else
            printf("input invalid, \n");
    }
}
