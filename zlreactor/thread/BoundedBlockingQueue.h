// ***********************************************************************
// Filename         : BoundedBlockingQueue.h
// Author           : LIZHENG
// Created          : 2014-06-08
// Description      : 固定大小的同步阻塞队列，可工作于多线程环境下，可用于线程之间数据存取
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_BOUNDEDBLOCKINGQUEUE_H
#define ZL_BOUNDEDBLOCKINGQUEUE_H
#include "zlreactor/Define.h"
#include <queue>
#include "zlreactor/thread/Mutex.h"
#include "zlreactor/thread/Condition.h"
#include "zlreactor/thread/BlockingQueue.h"

NAMESPACE_ZL_THREAD_START

//struct tagFIFO {};  //先进先出
//struct tagFILO {};  //先进后出
//struct tagPRIO {};  //按优先级

template <typename Job, typename Queue = std::queue<Job>, typename Order = tagFIFO >
class BoundedBlockingQueue : public zl::NonCopy
{
public:
    typedef Job                                 JobType;
    typedef Queue                               QueueType;
    typedef zl::thread::Mutex                   MutexType;
    typedef zl::thread::LockGuard<MutexType>    LockGuard;
    typedef zl::thread::Condition               ConditionType;

public:
    explicit BoundedBlockingQueue(int maxSize)
        : stopFlag_(false)
        , maxSize_(maxSize)
        , mutex_()
        , notEmpty_(mutex_)
        , notFull_(mutex_)
    {
    }

    ~BoundedBlockingQueue()
    {
        stop();
    }

public:
    bool push(const JobType& job)
    {
        LockGuard lock(mutex_);
        while(queue_.size() == maxSize_ && !stopFlag_)  // already full or stopped
        {
            notFull_.wait();
        }
        if(stopFlag_)
            return false;
                
        queue_.push(job);
        notEmpty_.notify_one();
        return true;
    }

    bool push(JobType&& job)
    {
        LockGuard lock(mutex_);
        while(queue_.size() == maxSize_ && !stopFlag_)  // already full or stopped
        {
            notFull_.wait();
        }
        if(stopFlag_)
            return false;
            
        queue_.push(std::move(job));
        notEmpty_.notify_one();
        return true;
    }

    bool pop(JobType& job)
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

    JobType pop()
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

    bool pop(std::vector<JobType>& vec, int pop_size = -1)
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

        if(pop_size <= 0)
             pop_size = queue_.size();

        JobType job;
        while (pop_size-- > 0 && !stopFlag_)
        {
            if(!popOne(job, Order()))
                break;
            else
                vec.push_back(job);
        }

        return true;
    }

    bool try_pop(JobType& job)
    {
        LockGuard lock(mutex_);
        if(queue_.empty() || stopFlag_)
            return false;
        popOne(job, Order());
        notFull_.notify_one();
        return true;
    }

    void stop()
    {
        {
            LockGuard lock(mutex_);
            stopFlag_ = true;
        }
        notFull_.notify_all();
        notEmpty_.notify_all();
    }

    size_t size() const
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

    //template <>
    bool popOne(JobType& job, tagFIFO tag)
    {
        if(queue_.empty())
            return false;
        job = queue_.front();
        queue_.pop();
        return true;
    }

    //template <>
    bool popOne(JobType& job, tagFILO tag)
    {
        if(queue_.empty())
            return false;
        job = queue_.top();
        queue_.pop();
        return true;
    }

    //template <>
    bool popOne(JobType& job, tagPRIO tag)
    {
        if(queue_.empty())
            return false;
        job = queue_.top();
        queue_.pop();
        return true;
    }

protected:
    bool                    stopFlag_;
    int                     maxSize_;
    mutable MutexType       mutex_;
    ConditionType           notEmpty_;
    ConditionType           notFull_;
    QueueType               queue_;
};


#if defined(ZL_CXX11_ENABLED) || (_MSC_VER >= 1700)   /// using is not support in VS2010

template <typename Job>
using FifoBoundedBlockingQueue = BoundedBlockingQueue<Job, std::queue<Job>, tagFIFO>;

template <typename Job>
using FiloBoundedBlockingQueue = BoundedBlockingQueue<Job, std::stack<Job>, tagFILO>;

template <typename Job>
using PrioBoundedBlockingQueue = BoundedBlockingQueue<Job, std::priority_queue<Job>, tagPRIO>;

#endif

NAMESPACE_ZL_THREAD_END
#endif /* ZL_BOUNDEDBLOCKINGQUEUE_H */
