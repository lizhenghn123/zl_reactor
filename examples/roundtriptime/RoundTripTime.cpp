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

// roundtrip ：测量两台机器的网络延迟，即“往返时间 / round trip time / RTT”
// see http://blog.csdn.net/solstice/article/details/6335082

const size_t frameLen = 2 * sizeof(int64_t);

namespace ts
{
void serverConnectionCallback(const TcpConnectionPtr& conn)
{
    cout << conn->peerAddress().ipPort() << " -> " << conn->localAddress().ipPort()
         << " is " << (conn->connected() ? "UP" : "DOWN") << "\n";

    if (conn->connected())
    {
        conn->setNoDelay(true);
    }
    else
    {
    }
}

void serverMessageCallback(const TcpConnectionPtr& conn, ByteBuffer* buffer, Timestamp receiveTime)
{
    static int64_t i = 100;
    int64_t message[2];
    while (buffer->readableBytes() >= frameLen)
    {
        memcpy(message, buffer->peek(), frameLen);
        buffer->retrieve(frameLen);
        message[1] = receiveTime.microSeconds();
        cout << "server get/send data time : " << message[1] << "\t" << message[0] << "\n";
        conn->send(message, sizeof(message));
    }
}

void runServer(const char* ip, uint16_t port)
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(ip, port), "ClockServer");
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

void clientMessageCallback(const TcpConnectionPtr&, ByteBuffer* buffer, Timestamp receiveTime)
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
        cout << "client get  data time : " << back << " " <<  receiveTime.toString() << "\t"
             << "round trip " << back - send << ", clock error " << their - mine << "\n";
        // 同一台机器上运行server和client，其时间差理论上为0，可结果却不是0，时正时负
    }
}

void sendMyTime()
{
    static int64_t i = 0;
    if (clientConnection)
    {
        int64_t message[2] = { 0, 0 };
        Timestamp now = Timestamp::now();
        message[0] = now.microSeconds();  
        cout << "client send data time : " << message[0] << " " << now.toString() <<"\n"; 
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
    loop.addTimer(sendMyTime, 1, true);   //启动一个每1s运行一次的定时器
    loop.loop();
}

}

void printUsage(int argc, char* argv[])
{
    printf("-------- test roundtrip --------\n");
    printf("run server : %s -s <ip> <port>\n", argv[0]);
    printf("run client : %s -c <ip> <port>\n", argv[0]);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        printUsage(argc, argv);
    }

    zl::base::Logger::setLogPriority(zl::base::ZL_LOG_PRIO_ALERT);

    uint16_t port = static_cast<uint16_t>(atoi(argv[3]));
    if (strcmp(argv[1], "-s") == 0)
    {
        ts::runServer(argv[2], port);
    }
    else if (strcmp(argv[1], "-c") == 0)
    {
        tc::runClient(argv[2], port);
    }
	else
	{
		printUsage(argc, argv);
	}
}
