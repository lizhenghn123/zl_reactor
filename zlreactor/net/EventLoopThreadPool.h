// ***********************************************************************
// Filename         : EventLoopThreadPool.h
// Author           : LIZHENG
// Created          : 2014-11-07
// Description      : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_EVENTLOOPTHREADPOOL_H
#define ZL_EVENTLOOPTHREADPOOL_H
#include "zlreactor/Define.h"
#include "zlreactor/base/NonCopy.h"
#include "zlreactor/net/SocketUtil.h"
#include "zlreactor/net/InetAddress.h"
#include "zlreactor/thread/Mutex.h"
namespace zl
{
    namespace thread
    {
        class Thread;
        class CountDownLatch;
    }
}
NAMESPACE_ZL_NET_START
class EventLoop;

class EventLoopThreadPool : zl::NonCopy
{
public:
    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();

    std::vector<EventLoop*> getAllLoops() { return loops_; }

    /// 设置EventLoopThreadPool的threads大小；if numThreads
    /// < 0  : 设置该值为当前系统CPU并发数；
    /// == 0 : 不使用EventLoopThreadPool，所有Channel都在同一个EventLoop中运行，默认值；
    /// > 0  : 设置numThreads个线程，也即numThreads个EventLoop，每个连接选择其中一个
    void setMultiReactorThreads(int numThreads);

    void start();
    bool isStart() { return started_; }
    EventLoop* getNextLoop();

private:
    void runLoop();

private:
    EventLoop                  *baseLoop_;
    bool                       started_;
    int                        numThreads_;
    size_t                     next_;
    zl::thread::Mutex          mutex_;
    zl::thread::CountDownLatch *latch_;
    std::vector<EventLoop*>    loops_;
    std::vector<zl::thread::Thread*>     threads_;
};

NAMESPACE_ZL_NET_END
#endif  /* EVENTLOOPTHREADPOOL */
