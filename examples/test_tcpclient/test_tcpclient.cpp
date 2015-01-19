#include <iostream>
#include <stdio.h>
#include "base/ZLog.h"
#include "net/EventLoop.h"
#include "net/TcpClient.h"
#include "net/TcpServer.h"
#include "net/TcpConnection.h"
#include "net/NetBuffer.h"
#include "net/NetUtil.h"
using namespace std;
using namespace zl;
using namespace zl::base;
using namespace zl::thread;
using namespace zl::net;


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

void clientMessageCallback(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp receiveTime)
{
    string msg(buf->retrieveAllAsString());
    //cout << "EchoServer::onMessage, fd [" << conn->fd() << "], "<< msg.size() << " bytes, ["
    //    << msg.data() << "], received at " << time.toString() << "\n";
    cout << "client recv data : " << msg << " at " <<receiveTime.toString() << "\n";
    //conn->send(msg.data(), msg.size());
}

void send_data()
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

void runClient(const char* ip, uint16_t port)
{
    EventLoop loop;
    TcpClient client(&loop, InetAddress(ip, port)); 
    client.enableRetry();
    client.setConnectionCallback(clientConnectionCallback);
    client.setMessageCallback(clientMessageCallback);
    client.connect();
    loop.addTimer(send_data, 1, true);   //启动一个每1s运行一次的定时器
    loop.loop();
}


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Usage : %s <server_port>\n", argv[0]);
        return 0;
    }
    runClient("127.0.0.1", atoi(argv[1]));
}
