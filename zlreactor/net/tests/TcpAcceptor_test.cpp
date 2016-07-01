/*************************************************************************
	File Name   : TcpAcceptor_test.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年05月25日 星期一 20时45分36秒
 ************************************************************************/
#include <iostream>
#include <unistd.h>
#include "zlreactor/net/TcpAcceptor.h"
#include "zlreactor/net/InetAddress.h"
#include "zlreactor/net/EventLoop.h"
using namespace std;
using namespace zl;
using namespace zl::net;

void accept_callback(int fd, const InetAddress& addr)
{
    cout << "get " << fd << ", and close it.\n";
	::close(fd);
}

void test_tcpacceptor()
{
    EventLoop loop;
    InetAddress serverAddr(8888);
    TcpAcceptor acceptor(&loop, serverAddr);
    acceptor.setNewConnectionCallback(accept_callback);
    acceptor.listen();

    loop.loop();
}

int main()
{
    test_tcpacceptor();
}
