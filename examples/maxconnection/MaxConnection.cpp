#include <iostream>
#include "zlreactor/net/TcpServer.h"
#include "zlreactor/net/TcpConnection.h"
#include "zlreactor/net/EventLoop.h"
#include "zlreactor/base/Logger.h"
#include "zlreactor/thread/Atomic.h"
using namespace std;
using namespace zl::net;
using namespace zl::thread;

// 限制并发连接数
// see http://blog.csdn.net/solstice/article/details/6365666
class EchoServer
{
public:
    EchoServer(EventLoop* loop, const InetAddress& listenAddr);

    void start();

    void setMaxConnections(int conns) { kMaxConnections_ = conns; }

private:
    void onConnection(const TcpConnectionPtr& conn);

    void onMessage(const TcpConnectionPtr& conn, ByteBuffer* buf, Timestamp time);

    TcpServer server_;
    Atomic<int>  numConnected_;
    int kMaxConnections_;
};

EchoServer::EchoServer(EventLoop* loop, const InetAddress& listenAddr)
    : server_(loop, listenAddr, "EchoServer")
{
    server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, 
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void EchoServer::start()
{
    server_.setMultiReactorThreads(4);
    server_.start();
}

void EchoServer::onConnection(const TcpConnectionPtr& conn)
{
    cout  << "EchoServer::onConnection " << conn->peerAddress().ipPort() << " is "
        << (conn->connected() ? "UP" : "DOWN") << "\n"; 
    if (conn->connected()) // connecting
    {
        ++numConnected_;
        if (numConnected_ > kMaxConnections_)
        {
            conn->shutdown();
            //conn->forceCloseWithDelay(3.0);  // > round trip of the whole Internet.
            cout << "cannot accept this client[" << conn->fd() << "], closing..." << "\n";
        }
    }
    else    // closeing
    {
        --numConnected_;
    }
    cout << "numConnected = " << numConnected_ << "\n";
}

void EchoServer::onMessage(const TcpConnectionPtr& conn, ByteBuffer* buf, Timestamp time)
{
    string msg(buf->retrieveAllAsString());
    //cout << "EchoServer::onMessage, fd [" << conn->fd() << "], "<< msg.size() << " bytes, ["
    //    << msg.data() << "], received at " << time.toString() << "\n";
    conn->send(msg.data(), msg.size());
}

int main(int argc, char* argv[])
{
    printf("----------- test max connection -----------\n");
    printf("usage : %s, or %s <int>\n", argv[0], argv[0]);

    int maxConnections = 5;
    if (argc > 1)
    {
        maxConnections = atoi(argv[1]);
    }
    printf("now, the server can hold max connection is [%d]\n", maxConnections);

    zl::base::Logger::setLogPriority(zl::base::ZL_LOG_PRIO_ALERT);

    EventLoop loop;
    InetAddress listenAddr("127.0.0.1", 8888);
    EchoServer server(&loop, listenAddr);
    server.setMaxConnections(maxConnections);

    server.start();
    loop.loop();
}
