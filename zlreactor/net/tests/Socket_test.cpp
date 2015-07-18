/*************************************************************************
	File Name   : TcpAcceptor_test.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年06月23日 星期二 21时50分36秒
 ************************************************************************/
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include "net/Socket.h"
#include "net/SocketUtil.h"
#include "thread/Thread.h"
using namespace std;
using namespace zl;
using namespace zl::net;
using namespace zl::thread;

void test_socket()
{
    {
        Socket sock(SocketUtil::createSocket());
        assert(sock.fd() > 0);
        sock.setReuseAddr(true);
        bool ret = sock.bind("0.0.0.0", 8888);
        assert(ret);
        ret = sock.listen();
        assert(ret);
    }
    {
        Socket sock(SocketUtil::createSocketAndListen("0.0.0.0", 8888, 1024));
        assert(sock.fd() > 0);
    }
}

namespace detail
{
    const char* g_serverIp = "0.0.0.0";
    const int   g_serverPort = 8888;

    void server_thread()
    {
        Socket sock(SocketUtil::createSocketAndListen(g_serverIp, g_serverPort, 1024));
        assert(sock.fd() > 0);
        ZL_SOCKADDR_IN peerAddr;
        std::vector<Socket*> sockets;
        printf("server socket is listening......\n");
        while (true)
        {
            memset(&peerAddr, 0, sizeof(peerAddr));
            int clifd = sock.accept(&peerAddr);
            assert(clifd);
            Socket* client = new Socket(clifd);
            printf("server accept [%d] from [%s]\n", client->fd(), SocketUtil::getPeerIpPort(client->fd()).c_str());

            sockets.push_back(client);
        }
    }

    void client_thread()
    {
        this_thread::sleep(1000);
        int max_count = 10;
        while (max_count-- > 0)
        {
            Socket sock(SocketUtil::createSocket());
            assert(sock.fd() > 0);

            bool ret = sock.connect(g_serverIp, g_serverPort);
            printf("client connect [%s]\n", ret ? "success" : "failure");
        }
        printf("client_thread is over!\n");
    }
}

void test_server_client()
{
    Thread srvtrd(detail::server_thread);
    Thread clitrd(detail::client_thread);
    srvtrd.join();
    clitrd.join();
}
int main()
{
    test_socket();

    test_server_client();

    printf("###### GAME OVER ######\n");
}
