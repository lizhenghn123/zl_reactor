#include <iostream>
#include <assert.h>
#include "net/EventLoop.h"
#include "thread/Thread.h"
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
const static int test_flag = 2;
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
        //t3::test();
    }
    else
    {
        printf("---test nothing---\n");
    }
    printf("-----------------------------\n");

    return 0;
}
