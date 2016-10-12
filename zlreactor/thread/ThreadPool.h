// ***********************************************************************
// Filename         : ThreadPool.h
// Author           : LIZHENG
// Created          : 2014-10-06
// Description      :
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_THREADPOOL_H
#define ZL_THREADPOOL_H
#include "zlreactor/Define.h"
#include "zlreactor/base/NonCopy.h"
#include "zlreactor/thread/BlockingQueue.h"
NAMESPACE_ZL_THREAD_START

class Thread;

class ThreadPool : zl::NonCopy
{
public:
    typedef std::function<void ()> Task;

public:
    explicit ThreadPool(const std::string& name = "ThreadPool");
    ~ThreadPool();

public:
    void start(int numThreads);
    void stop();
    void run(const Task& f);
    size_t size() const { return queue_.size(); }

private:
    void executeThread();

private:
    std::string           name_;
    volatile bool         running_;
    BlockingQueue<Task>   queue_;
    std::vector<Thread *> threads_;
};

NAMESPACE_ZL_THREAD_END
#endif  /* ZL_THREADPOOL_H */
