/*************************************************************************
	File Name   : main.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年05月12日 星期二 19时57分02秒
 ************************************************************************/
#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/NetBuffer.h"
#include "net/TcpConnection.h"
#include "net/TcpClient.h"
#include "base/ZLog.h"
#include "EchoServer.h"
#include "EchoClient.h"

using namespace std;
using namespace zl;
using namespace zl::base;
using namespace zl::net;

void runServerLoop(int port, int reactor_threads)
{
    EventLoop loop;
    InetAddress listenAddr(port);

    EchoServer server(&loop, listenAddr, reactor_threads);

    printf("server listening on %s.....\n", listenAddr.ipPort().c_str());

    server.start();
    loop.loop();
}

vector<EchoClient*> g_clients;
void runClientLoop(const char *ip, int port, int clientNum)
{
    assert(clientNum > 0);

    EventLoop loop;
    InetAddress serverAddr(ip, port);

    for (int i = 1; i <= clientNum; ++i)
    {
        char buf[32] = { 0 };
        snprintf(buf, sizeof(buf), "EchoClient%d", i);
		cout << buf << "\n";
        EchoClient *client = new EchoClient(&loop, serverAddr, buf);
        g_clients.push_back(client);
    }

	srand((unsigned)time(NULL));
    for (int i = 0; i < g_clients.size(); ++i)
    {
        g_clients[i]->connect();
		int seconds = rand() % 10;
		//LOG_INFO("socket [%d] would be stop after [%d] seconds", g_clients[i]->fd(), seconds);
		loop.addTimer(std::bind(&EchoClient::stop, g_clients[i]), seconds);
    }

    loop.loop();

}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("*** usage ***\n");
        printf("run echoserver: %s -s <listen port> <reactors num>\n", argv[0]);
        printf("run echoclient: %s <server ip> <server port> <clients num>\n", argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "-s") == 0)
    {
        runServerLoop(atoi(argv[2]), atoi(argv[3]));
    }
    else
    {
        cout << argv[1] << "\t" << atoi(argv[2]) << "\t" << atoi(argv[3]) << "\n";
        runClientLoop(argv[1], atoi(argv[2]), atoi(argv[3]));
    }

    return 0;
}

