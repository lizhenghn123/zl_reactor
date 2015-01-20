#include <iostream>
#include <map>
#include <string>
#include "net/EventLoop.h"
#include "net/TcpServer.h"
#include "net/TcpConnection.h"
#include "net/NetBuffer.h"
using namespace std;
using namespace zl;
using namespace zl::net;

// Finger 协议:
//>> finger lizheng lizheng@192.168.14.6
//Login: lizheng                    Name: lizheng
//Directory: /home/lizheng              Shell: /bin/bash
//           On since 一 1月 12 10:23 (CST) on tty1 from :0
//           3 days 6 hours idle
//           On since 一 1月 12 11:43 (CST) on pts/1 from :0.0
//           1 day idle
//           No mail.
//           No Plan.

// Python Twisted 库实现的finger tutorial 请参见 ：
// http://twistedmatrix.com/documents/current/core/howto/tutorial/intro.html

// see http://blog.csdn.net/solstice/article/details/6203581

// Test01 ：Refuse Connections
// just only run the reactor, no listen or accept or read or write
void finger01()
{
    EventLoop loop;
    loop.loop();
}

// Test02 ：Do Nothing
// listen on 1079, accept socket, but do nothing
void finger02()
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(1079), "Finger02");
    server.start();
    loop.loop();
}

// Test03 ：Drop Connections
//接受新连接之后主动断开
void onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
        conn->shutdown();
    }
}
void finger03()
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(1079), "Finger03");
    server.setConnectionCallback(onConnection);
    server.start();
    loop.loop();
}

// Test04 ：Read Username, Drop Connections
//如果读到一行以 /r/n 结尾的消息，就断开连接
//注意这段代码有安全问题，如果恶意客户端不断发送数据而不换行，会撑爆服务端的内存。
//另外，Buffer::findCRLF() 是线性查找，如果客户端每次发一个字节，服务端的时间复杂度为 O(N^2)，会消耗 CPU 资源。
void onMessage4(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp receiveTime)
{
    if (buf->findCRLF() || buf->readableBytes() > 100)
    {
        conn->shutdown();
    }
}
void finger04()
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(1079), "Finger04");
    server.setMessageCallback(onMessage4);
    server.start();
    loop.loop();
}

// Test05 ：Read Username, Output Error, Drop Connections
//读取用户名、输出错误信息、然后断开连接
void onMessage5(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp receiveTime)
{
    if (buf->findCRLF())
    {
        conn->send("No such user\r\n");
        conn->shutdown();
    }
}
void finger05()
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(1079), "Finger");
    server.setMessageCallback(onMessage5);
    server.start();
    loop.loop();
}

// Test06 ：Output From Empty Factory
void onMessage6(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp receiveTime)
{
    if (buf->findCRLF())
    {
        conn->send("No such user\r\n");
        conn->shutdown();
    }
}
void finger06()
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(1079), "Finger06");
    server.setMessageCallback(onMessage6);
    server.start();
    loop.loop();
}

void finger07()
{
    EventLoop loop;
    loop.loop();
}
void finger08()
{
    EventLoop loop;
    loop.loop();
}

int main(int argc, char* argv[])
{
    printf("usage: %s <int: 1-8>\n", argv[0]);

    int choice = 1;
    while(1)
    {
        printf("input your choice : ");
        scanf("%d", &choice);

        if(choice == 0)
            continue;
        else if(choice == 1)
            finger01();
        else if(choice == 2)
            finger02();
        else if(choice == 3)
            finger03();
        else if(choice == 4)
            finger04();
        else if(choice == 5)
            finger05();
        else if(choice == 6)
            finger06();
        else if(choice == 7)
            finger07();
        else if(choice == 8)
            finger08();
        else
            printf("input error\n");
    }
}

/******
test finger06:

[root@lzv6 bin]# telnet localhost 1079 
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
asdfg
No such user
Connection closed by foreign host.

******/