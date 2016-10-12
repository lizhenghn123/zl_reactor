#include <iostream>
#include <stdio.h>
#include "zlreactor/base/Logger.h"
#include "zlreactor/net/EventLoop.h"
#include "zlreactor/net/TcpClient.h"
#include "zlreactor/net/TcpServer.h"
#include "zlreactor/net/TcpConnection.h"
#include "zlreactor/net/ByteBuffer.h"
#include "zlreactor/net/NetUtil.h"
using namespace std;
using namespace zl;
using namespace zl::base;
using namespace zl::thread;
using namespace zl::net;


namespace test_client
{
    TcpConnectionPtr clientConnection;

    void clientConnectionCallback(const TcpConnectionPtr& conn)
    {
        cout << conn->peerAddress().ipPort() << " -> " << conn->localAddress().ipPort()
            << " is " << (conn->connected() ? "UP" : "DOWN") << "\n";
        if (conn->connected())
        {
            clientConnection = conn;
            conn->setNoDelay(true);
        }
        else
        {
            clientConnection.reset();
        }
    }

    void clientMessageCallback(const TcpConnectionPtr& conn, ByteBuffer* buf, Timestamp receiveTime)
    {
        string msg(buf->retrieveAllAsString());
        //cout << "EchoServer::onMessage, fd [" << conn->fd() << "], "<< msg.size() << " bytes, ["
        //    << msg.data() << "], received at " << time.toString() << "\n";
        cout << "client recv data : " << msg << " at " <<receiveTime.toString() << "\n";
        //conn->send(msg.data(), msg.size());
    }

    void sendDataByClient()
    {
        static int64_t i = 0;
        if (clientConnection)
        {
            Timestamp now = Timestamp::now();
            char str[] = "hello world";
            cout << "client send data : " << str << " at " << now.toString() <<"\n";
            clientConnection->send(str, sizeof(str));
        }
    }
}

// 测试TcpClient, 标准输入加入到EventLoop进行管理
namespace test_client_stdin
{
    class TestTcpClient
    {
    public:
        TestTcpClient(EventLoop* loop, const InetAddress& listenAddr)
            : loop_(loop), client_(loop, listenAddr, "TestTcpClient")
        {
            client_.setConnectionCallback(std::bind(&TestTcpClient::onConnection, this, std::placeholders::_1));
            client_.setMessageCallback(std::bind(&TestTcpClient::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));
            // 标准输入缓冲区中有数据的时候，回调TestTcpClient::handleRead
            stdinChannel_ = new Channel(loop, 0);
            stdinChannel_->setReadCallback(std::bind(&TestTcpClient::handleRead, this));
            stdinChannel_->enableReading();		// 关注可读事件
        }
        ~TestTcpClient()
        {
            delete stdinChannel_;
        }
        void connect()
        {
            client_.connect();
        }
    private:
        void onConnection(const TcpConnectionPtr& conn)
        {
            cout << conn->peerAddress().ipPort() << " -> " << conn->localAddress().ipPort()
                << " is " << (conn->connected() ? "UP" : "DOWN") << "\n";
            clientConnection = conn;
            if (conn->connected())
            {
                //conn->setNoDelay(true);
            }
            else
            {
                //clientConnection.reset();
            }
        }
        void onMessage(const TcpConnectionPtr& conn, ByteBuffer *buf, Timestamp time)
        {
            string msg(buf->retrieveAllAsString());
            printf("onMessage(): recv a message [%s]\n", msg.c_str());
            LOG_INFO("[%d] recv %d bytes at %s", conn->fd(), msg.size(), time.toString().c_str());
        }
        // 标准输入缓冲区中有数据的时候，回调该函数
        void handleRead()
        {
            char buf[1024] = { 0 };
            fgets(buf, 1024, stdin);
            buf[strlen(buf) - 1] = '\0';		// 去除\n
            clientConnection->send(buf);
        }
    private:
        EventLoop *loop_;
        TcpClient client_;
        Channel *stdinChannel_;		// 标准输入Channel
        TcpConnectionPtr clientConnection;
    };
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Usage : %s <server_port>\n", argv[0]);
        return 0;
    }

    EventLoop loop;
    InetAddress serverAddr("127.0.0.1", atoi(argv[1]));

    const static int flag = 2;
    if (flag == 1)
    {
        TcpClient client(&loop, serverAddr);
        client.enableRetry();
        client.setConnectionCallback(test_client::clientConnectionCallback);
        client.setMessageCallback(test_client::clientMessageCallback);
        client.connect();
        loop.addTimer(test_client::sendDataByClient, 1, true);   //启动一个每1s运行一次的定时器
        loop.loop();
    }
    else if (flag == 2)
    {
        test_client_stdin::TestTcpClient client(&loop, serverAddr);
        client.connect();
        loop.loop();
    }
}
