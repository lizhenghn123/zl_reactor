// ***********************************************************************
// Filename         : BlockingQueueWorker.h
// Author           : LIZHENG
// Created          : 2014-05-16
// Description      : 工作调度，工作在阻塞队列(BlockingQueue 或者 BoundedBlockingQueue)上面
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_BLOCKQUEUEWORKER_H
#define ZL_BLOCKQUEUEWORKER_H
#include "thread/BlockingQueue.h"
#include "thread/ThreadGroup.h"
NAMESPACE_ZL_THREAD_START

template <typename Queue>
class BlockingQueueWorker
{
public:
    typedef Queue                               QueueType;
    typedef typename Queue::JobType             JobType;
    typedef std::function<void(JobType&)>       FunctionType;

    template<typename FunctionType>
    BlockingQueueWorker(QueueType& queue, const FunctionType& function, int thread_num = 1)
        : queue_(queue)
        , function_(function)
        , threadNum_(thread_num)
    {
    }

    BlockingQueueWorker(QueueType& queue, int thread_num = 1)
        : queue_(queue)
        , function_(NULL)
        , threadNum_(thread_num)
    {
    }

    ~BlockingQueueWorker()
    {
        stop();
    }

    void start()
    {
        if(threads_.size() > 0) return;
        for (int i = 0; i < threadNum_; ++i)
        {
            threads_.create_thread(std::bind(&BlockingQueueWorker::doWork, this));
        }
    }

    template<typename FunctionType>
    void start(const FunctionType& function)
    {
        function_ = function;
        start();
    }

    void stop()
    {
        function_ = 0;
        queue_.stop();
        threads_.join_all();
    }

private:
    void doWork()
    {
        for(;;)
        {
            JobType job;
            bool bret = queue_.pop(job);
            if (!bret) break;
            if (function_)
            {
                function_(job);
            }
        }
    }

private:
    QueueType&              queue_;
    FunctionType            function_;
    int                     threadNum_;
    ThreadGroup             threads_;
};    

NAMESPACE_ZL_THREAD_END
#endif  /* ZL_BLOCKQUEUEWORKER_H */
