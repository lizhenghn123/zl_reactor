#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include "zlreactor/net/TcpServer.h"
#include "zlreactor/net/TcpConnection.h"
#include "zlreactor/net/EventLoop.h"
#include "zlreactor/net/ByteBuffer.h"
#include "zlreactor/base/Logger.h"
#include "zlreactor/thread/Atomic.h"
using namespace std;
using namespace zl::net;
using namespace zl::thread;

void onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        conn->setNoDelay(true);
    }
}

const char* message = "ping pong server";
void onMessage(const TcpConnectionPtr& conn, ByteBuffer *buf, Timestamp)
{
    //conn->send(message);
    conn->send(buf);
}

int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        fprintf(stderr, "Usage: server <address> <port> <threads>\n");
    }
    else
    {
        LOG_SET_PRIORITY(zl::base::ZL_LOG_PRIO_WARNING);

        const char *ip = argv[1];
        uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
        InetAddress listenAddr(ip, port);
        int threadCount = atoi(argv[3]);

        EventLoop loop;
        TcpServer server(&loop, listenAddr, "PingPong");
        server.setConnectionCallback(onConnection);
        server.setMessageCallback(onMessage);

        if(threadCount > 1)
        {
            server.setMultiReactorThreads(threadCount);
        }

        server.start();

        loop.loop();
    }
}

