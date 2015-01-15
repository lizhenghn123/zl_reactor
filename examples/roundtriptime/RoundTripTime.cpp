#include <iostream>
#include <stdio.h>
#include "base/ZLog.h"
#include "net/EventLoop.h"
#include "net/TcpClient.h"
#include "net/TcpServer.h"
#include "net/TcpConnection.h"
#include "net/NetBuffer.h"
using namespace std;
using namespace zl;
using namespace zl::base;
using namespace zl::thread;
using namespace zl::net;

// roundtrip ：测量两台机器的网络延迟，即“往返时间 / round trip time / RTT”
// see http://blog.csdn.net/solstice/article/details/6335082

const size_t frameLen = 2 * sizeof(int64_t);

namespace ts
{
void serverConnectionCallback(const TcpConnectionPtr& conn)
{
    cout << conn->peerAddress().ipPort() << " -> " << conn->localAddress().ipPort()
         << " is " << (conn->connected() ? "UP" : "DOWN");

    if (conn->connected())
    {
        conn->setNoDelay(true);
    }
    else
    {
    }
}

void serverMessageCallback(const TcpConnectionPtr& conn, NetBuffer* buffer, Timestamp receiveTime)
{
    int64_t message[2];
    while (buffer->readableBytes() >= frameLen)
    {
        memcpy(message, buffer->peek(), frameLen);
        buffer->retrieve(frameLen);
        message[1] = receiveTime.microSeconds();
        conn->send(message, sizeof(message));
    }
}

void runServer(uint16_t port)
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(port), "ClockServer");
    server.setConnectionCallback(ts::serverConnectionCallback);
    server.setMessageCallback(ts::serverMessageCallback);
    server.start();
    loop.loop();
}

}

namespace tc
{

TcpConnectionPtr clientConnection;

void clientConnectionCallback(const TcpConnectionPtr& conn)
{
    cout << conn->peerAddress().ipPort() << " -> " << conn->localAddress().ipPort()
         << " is " << (conn->connected() ? "UP" : "DOWN");
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

void clientMessageCallback(const TcpConnectionPtr&, NetBuffer* buffer, Timestamp receiveTime)
{
    int64_t message[2];
    while (buffer->readableBytes() >= frameLen)
    {
        memcpy(message, buffer->peek(), frameLen);
        buffer->retrieve(frameLen);
        int64_t send = message[0];
        int64_t their = message[1];
        int64_t back = receiveTime.microSeconds();
        int64_t mine = (back+send)/2;
        cout << "round trip " << back - send << ", clock error " << their - mine;
    }
}

void sendMyTime()
{
    if (clientConnection)
    {
        int64_t message[2] = { 0, 0 };
        message[0] = Timestamp::now().microSeconds();
        clientConnection->send(message, sizeof(message));
    }
}

void runClient(const char* ip, uint16_t port)
{
    EventLoop loop;
    TcpClient client(&loop, InetAddress(ip, port), "ClockClient");
    client.enableRetry();
    client.setConnectionCallback(clientConnectionCallback);
    client.setMessageCallback(clientMessageCallback);
    client.connect();
    loop.addTimer(sendMyTime, 0.2, true);   //启动一个每0.2s运行一次的定时器
    loop.loop();
}

}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("-------- test roundtrip --------\n");
        //printf("Usage:\n%s -s port\n%s ip port\n", argv[0], argv[0]);
        printf("run server : %s -s <port>", argv[0]);
        printf("run client : %s <ip> <port>", argv[0]);
        exit(0);
    }
    if (argc > 2)
    {
        uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
        if (strcmp(argv[1], "-s") == 0)
        {
            ts::runServer(port);
        }
        else
        {
            tc::runClient(argv[1], port);
        }
    }
}
