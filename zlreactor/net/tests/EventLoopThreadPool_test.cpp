/*************************************************************************
	File Name   : EventLoopThreadPool_test.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年05月11日 星期一 22时36分31秒
 ************************************************************************/
#include <iostream>
#include "zlreactor/net/EventLoop.h"
#include "zlreactor/net/EventLoopThreadPool.h"
using namespace std;
using namespace zl;
using namespace zl::net;

void test_eltp()
{
    EventLoop loop;
    loop.addTimer(std::bind(&EventLoop::quit, &loop), 10, false);

    EventLoopThreadPool eltp1(&loop);
    eltp1.setMultiReactorThreads(0); // no multi-reactor, only one EventLoop(loop), eltp1 create none.
    eltp1.start();
    assert(eltp1.getNextLoop() == &loop);
    assert(eltp1.getNextLoop() == &loop);
    assert(eltp1.getNextLoop() == &loop);

    EventLoopThreadPool eltp2(&loop);
    eltp2.setMultiReactorThreads(1); // multi-reactor(two), one (loop) for accept, one(eltp2 create) for other.
    eltp2.start();
    assert(eltp2.getNextLoop() != &loop);
    EventLoop *lp2 = eltp2.getNextLoop();
    assert(eltp2.getNextLoop() != &loop);
    assert(eltp2.getNextLoop() == lp2);

    EventLoopThreadPool eltp3(&loop);
    eltp3.setMultiReactorThreads(2); // multi-reactor(three), one (loop) for accept, two(eltp3 create) for other.
    eltp3.start();
    EventLoop *lp3 = eltp3.getNextLoop();
    assert(lp3 != &loop);
    assert(lp3 != eltp3.getNextLoop());
    assert(lp3 == eltp3.getNextLoop());
    assert(lp3 != eltp3.getNextLoop());

    loop.loop();
}

int main()
{
    test_eltp();
    cout << Timestamp::now().toString() << "\n";
    cout << "###### GAME OVER ######\n";
}
