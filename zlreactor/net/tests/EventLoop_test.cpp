#include <iostream>
#include <assert.h>
#include "net/EventLoop.h"
#include "thread/Thread.h"
#include "net/TcpAcceptor.h"
#include "net/InetAddress.h"
#include "net/SocketUtil.h"
using namespace std;
using namespace zl::thread;
using namespace zl::net;

namespace t1
{
    void threadFunc()
    {
        printf("threadFunc(): pid = %d, tid = %d\n", getpid(), this_thread::get_id().tid());
        EventLoop loop;
        loop.loop();
    }

    void test()
    {
        printf("main(): pid = %d, tid = %d\n", getpid(), this_thread::get_id().tid());
        EventLoop loop;
        Thread t(threadFunc);

        loop.loop();
        t.join();
    }
}

namespace t2
{
    EventLoop *g_loop;

    void threadFunc()
    {
        printf("threadFunc(): pid = %d, tid = %d\n", getpid(), this_thread::get_id().tid());
        g_loop->loop();  // ERROR, ASSERT FAILURE, loop must run in the thread where this object created
    }
    void test()
    {
        printf("main(): pid = %d, tid = %d\n", getpid(), this_thread::get_id().tid());
        EventLoop loop;
        g_loop = &loop;
        Thread t(threadFunc);
        t.join();
    }
}
namespace t3
{
	void newConnection(int sockfd, const InetAddress& peerAddr)
	{
		static char buf[] = { "I am server, accept you and close you." };
		printf("newConnection(): pid = %d, tid = %d\n", getpid(), this_thread::get_id().tid());
		printf("newConnection(): accepted a new connection from %s\n", peerAddr.ipPort().c_str());
		SocketUtil::write(sockfd, buf, sizeof(buf));
        SocketUtil::closeSocket(sockfd);
	}
	void test_acceptor()
	{
		printf("main(): pid = %d, tid = %d\n", getpid(), this_thread::get_id().tid());
		InetAddress listenAddr(8888);
		EventLoop loop;

		TcpAcceptor acceptor(&loop, listenAddr);
		acceptor.setNewConnectionCallback(newConnection);
		acceptor.listen();

		loop.loop();	
	}
}
const static int test_flag = 3;
int main()
{
    if(test_flag == 1)
    {
        t1::test();
    }
    else if(test_flag == 2)
    {
        t2::test();
    }
    if(test_flag == 3)
    {
        t3::test_acceptor();
    }
    else
    {
        printf("---test nothing---\n");
    }
    printf("-----------------------------\n");

    return 0;
}
