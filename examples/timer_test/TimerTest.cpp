#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include "net/EventLoop.h"
#include "thread/Thread.h"
using namespace zl;
using namespace zl::net;

// see https://github.com/chenshuo/muduo/blob/master/muduo/net/tests/TimerQueue_unittest.cc
// chenshuo真是大牛，自己水平高，写出来的代码也能让大家读懂，技术文章也很好，高山仰止
// 本项目代码即是参照chenshuo及其他开源项目而自己练手写的

int cnt = 0;
EventLoop *g_loop;

void printTid()
{
    printf("pid = %d, tid = %ld\n", getpid(), thread::this_thread::get_id().tid());
    printf("now %s\n", Timestamp::now().toString().c_str());
}

void print(const char *msg)
{
    printf("msg %s %s\n", Timestamp::now().toString().c_str(), msg);
    if (++cnt == 20)
    {
        g_loop->quit();
    }
}

void cancel(TimerId timer)
{
    g_loop->cancelTimer(timer);
    printf("cancelled at %s\n", Timestamp::now().toString().c_str());
}

int main()
{
    printTid();
    sleep(1);
    {
        EventLoop loop;
        g_loop = &loop;

        print("main");
        loop.addTimer(std::bind(print, "once1"), 1, false);
        loop.addTimer(std::bind(print, "once1"), 1.5, false);
        loop.addTimer(std::bind(print, "once2.5"), 2.5, false);
        loop.addTimer(std::bind(print, "once3.5"), 3.5, false);
        TimerId t45 = loop.addTimer(std::bind(print, "once4.5"), 4.5, false);
        loop.addTimer(std::bind(cancel, t45), 4.0, false);
        loop.addTimer(std::bind(cancel, t45), 4.8, false);
        loop.addTimer(std::bind(print, "every2"), 2, true);
        TimerId t3 = loop.addTimer(std::bind(print, "every3"), 3, true);
        loop.addTimer(std::bind(cancel, t3), 9.001, false);

        loop.loop();
        print("main loop exits");
    }
}
