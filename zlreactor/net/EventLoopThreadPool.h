// ***********************************************************************
// Filename         : EventLoopThreadPool.h
// Author           : LIZHENG
// Created          : 2014-11-07
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-11-07
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_EVENTLOOPTHREADPOOL_H
#define ZL_EVENTLOOPTHREADPOOL_H
#include "Define.h"
#include "base/NonCopy.h"
#include "net/SocketUtil.h"
#include "net/InetAddress.h"
#include "thread/Mutex.h"
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

    void setThreadNum(int numThreads);
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
