// ***********************************************************************
// Filename         : BoundedBlockingQueue.h
// Author           : LIZHENG
// Created          : 2014-06-08
// Description      : 固定大小的同步阻塞队列，可工作于多线程环境下，可用于线程之间数据存取
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-08-25
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_BOUNDEDBLOCKINGQUEUE_H
#define ZL_BOUNDEDBLOCKINGQUEUE_H
#include "Define.h"
#include <queue>
#include <stack>
#include "thread/Mutex.h"
#include "thread/Condition.h"
#include "thread/BlockingQueue.h"

NAMESPACE_ZL_THREAD_START

//struct tagFIFO {};  //先进先出
//struct tagFILO {};  //先进后出
//struct tagPRIO {};  //按优先级

template <typename Job, typename Queue = std::queue<Job>, typename Order = tagFIFO >
class BoundedBlockingQueue : public zl::NonCopy
{
public:
    typedef Job                                 JobType;
    typedef Queue	                            QueueType;
    typedef zl::thread::Mutex                   MutexType;
    typedef zl::thread::MutexLocker             LockGuard;
    typedef zl::thread::Condition               ConditionType;

public:
    explicit BoundedBlockingQueue(int maxSize)
        : stopFlag_(false),
        maxSize_(maxSize),
        mutex_(),
        notEmpty_(mutex_),
        notFull_(mutex_)
    {}
    virtual ~BoundedBlockingQueue()
    {
        stop();
    }

public:
    virtual void push(const JobType& job)
    {
        LockGuard lock(mutex_);
        while(queue_.size() == maxSize_)  // already full
        {
            notFull_.wait();
        }
        queue_.push(job);
        notEmpty_.notify_one();
    }

    virtual bool pop(JobType& job)
    {
        LockGuard lock(mutex_);
        while(queue_.empty() && !stopFlag_)
        {
            notEmpty_.wait();
        }
        if(stopFlag_)
        {
            return false;
        }
        popOne(job, Order());
        notFull_.notify_one();
        return true;
    }

    virtual JobType pop()
    {
        LockGuard lock(mutex_);
        while(queue_.empty() && !stopFlag_)
        {
            notEmpty_.wait();
        }
        if(stopFlag_)
        {
            return false;
        }
        JobType job;
        popOne(job, Order());
        notFull_.notify_one();
        return job;
    }

    virtual bool try_pop(JobType& job)
    {
        LockGuard lock(mutex_);
        if(queue_.empty() && !stopFlag_)
            return false;
        popOne(job, Order());
        notFull_.notify_one();
        return true;
    }

    virtual bool pop(std::vector<JobType>& vec, int pop_size = 1)
    {
        LockGuard lock(mutex_);
        while(queue_.empty() && !stopFlag_)
        {
            notEmpty_.wait();
        }
        if(stopFlag_)
        {
            return false;
        }
        int num = 0;
        while (num < pop_size)
        {
            JobType job;
            if(!popOne(job, Order()))
                break;
            else
            {
                num ++;
                vec.push_back(job);
            }
        }
        return true;
    }

    virtual void stop()
    {
        stopFlag_ = true;
        notFull_.notify_all();
        notEmpty_.notify_all();
    }

    size_t size()
    {
        LockGuard lock(mutex_);
        return queue_.size();
    }

    bool empty()
    {
        LockGuard lock(mutex_);
        return queue_.empty();
    }

    bool full()
    {
        LockGuard lock(mutex_);
        return queue_.size() == maxSize_;
    }

    template < typename Func >
    void foreach(const Func& func)
    {
        LockGuard lock(mutex_);
        std::for_each(queue_.begin(), queue_.end(), func);
    }

private:
    template <typename T>
    bool popOne(JobType& job, T tag);

    template <>
    bool popOne(JobType& job, tagFIFO tag)
    {
        job = queue_.front();
        queue_.pop();
        return true;
    }

    template <>
    bool popOne(JobType& job, tagFILO tag)
    {
        job = queue_.top();
        queue_.pop();
        return true;
    }

    template <>
    bool popOne(JobType& job, tagPRIO tag)
    {
        job = queue_.top();
        queue_.pop();
        return true;
    }

protected:
    bool             stopFlag_;
    int              maxSize_;
    MutexType        mutex_;
    ConditionType    notEmpty_;
    ConditionType    notFull_;
    QueueType        queue_;
};

/* using is not support in VS2010*/
//template< typename Job>
//using BoundedBlockingFifoQueue = zl::BoundedBlockingQueue<Job, std::queue<Job>, zl::tagFIFO>;

//template< typename Job>
//using BoundedBlockingFiloQueue = zl::BoundedBlockingQueue<Job, std::stack<Job>, zl::tagFILO>;
//
//template< typename Job>
//using BoundedBlockingPrioQueue = zl::BoundedBlockingQueue<Job, std::priority_queue<Job>, zl::tagPRIO>;

NAMESPACE_ZL_THREAD_END
#endif /* ZL_BOUNDEDBLOCKINGQUEUE_H */