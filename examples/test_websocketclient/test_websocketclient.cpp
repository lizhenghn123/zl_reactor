#include <iostream>
#include <string>
#include "zlreactor/net/websocket/WebSocketClient.h"
#include "zlreactor/net/EventLoop.h"
#include "zlreactor/net/TcpConnection.h"
#include "zlreactor/base/FileUtil.h"
#include "zlreactor/base/Logger.h"
#include "zlreactor/base/LogFile.h"
using namespace std;
using namespace zl::net;
using namespace zl::net::ws;

zl::base::LogFile logfile("wsclient", ".");

void onOpen(const TcpConnectionPtr& conn)
{
    LOG_INFO("onOpen %s <-> %s is %s", conn->peerAddress().ipPort().c_str(), 
                conn->localAddress().ipPort().c_str(), (conn->connected() ? "UP" : "DOWN"));
    if (conn->connected())
    {
        //conn->send("hello world\n");
    }
    else
    {
        conn->shutdown();
    }
}

void onClose(const TcpConnectionPtr& conn)
{
    LOG_INFO("onClose");
}

void onMessage(const TcpConnectionPtr& conn, const std::vector<char>& buf, const Timestamp& time)
{
    LOG_INFO("onMessage");
}

class EchoWebClient
{
public:
    EchoWebClient(EventLoop *loop, const InetAddress& listenAddr, const std::string url)
        : client_(loop, listenAddr, url)
    {
        client_.setOnOpen(std::bind(&EchoWebClient::onopen, this, std::placeholders::_1));
        client_.setOnClose(std::bind(&EchoWebClient::onclose, this, std::placeholders::_1));
        client_.setOnMessage(std::bind(&EchoWebClient::onmessage, this, std::placeholders::_1,
                                std::placeholders::_2, std::placeholders::_3));
    }

    void connect()
    {
        client_.connect();
    }

private:
    void onopen(const TcpConnectionPtr& conn)
    {
        LOG_INFO("EchoWebClient (%d) onopen\n", conn->fd());
        if (conn->connected())
        {
            //client_.sendText(conn, "hello", 5);
            std::string ss(128, 'a');   /// FIXME 超过126时，server端无法解析！！！
            client_.sendText(conn, ss.c_str(), ss.size());
        }
        else
        {
            conn->shutdown();
        }
    }

    void onclose(const TcpConnectionPtr& conn)
    {
        LOG_INFO("EchoWebClient (%d) onclose\n", conn->fd());
        client_.close(conn);
    }

    void onmessage(const TcpConnectionPtr& conn, const std::vector<char>& buf, Timestamp)
    {
         LOG_INFO("EchoWebClient onmessage (%s)", buf.data());
         //std::string ss(128, 'a');
         //client_.sendText(conn, ss.c_str(), ss.size());
         //client_.sendText(conn, buf.data(), buf.size());
    }

private:
    zl::net::ws::WsClient client_;
};

int main()
{
    LOG_SET_DEBUG_MODE;

    EventLoop loop;
    InetAddress addr("0.0.0.0", 8888);
    
    #if 0
    zl::net::ws::WsClient client(&loop, addr, "/");
    client.setOnOpen(onOpen);
    client.setOnClose(onClose);
    client.setOnMessage(onMessage);
    client.connect();
    #else
    EchoWebClient client(&loop, addr, "/");
    client.connect();
    #endif

    loop.loop();
    return 0;
}
