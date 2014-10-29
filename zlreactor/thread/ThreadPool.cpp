#include "thread/ThreadPool.h"
#include "thread/Thread.h"
#include <assert.h>
NAMESPACE_ZL_THREAD_START

ThreadPool::ThreadPool(const std::string& name/* = "ThreadPool"*/)
    : mutex_(), cond_(mutex_), name_(name), running_(false)
{
}

ThreadPool::~ThreadPool()
{
    if (running_)
    {
        stop();
    }
}

void ThreadPool::start(int numThreads)
{
    if (running_)
        return;
    running_ = true;
    assert(threads_.empty());
    threads_.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i)
    {
        char id[32];
        ZL_SNPRINTF(id, sizeof id, "%d", i);
        threads_.push_back(new Thread(std::bind(&ThreadPool::executeThread, this), name_ + id));
    }
}

void ThreadPool::stop()
{
    running_ = false;
    cond_.notify_all();
    for_each(threads_.begin(), threads_.end(), std::bind(&Thread::join, std::placeholders::_1));
}

void ThreadPool::run(const Task& task)
{
    if (threads_.empty())
    {
        task();
    }
    else
    {
        MutexLocker guard(mutex_);
        queue_.push_back(task);
        cond_.notify_one();
    }
}

void ThreadPool::executeThread()
{
    try
    {
        while (running_)
        {
            Task task(popOne());
            if (task)
            {
                /*bool ret = */task();
            }
        }
    }
    catch (...)
    {
        //print("caught exception caught in ThreadPool %s\n", name_.c_str());
        std::abort();
    }
}

ThreadPool::Task ThreadPool::popOne()
{
    MutexLocker guard(mutex_);
    while (queue_.empty() && running_)
    {
        cond_.wait();
    }
    if(!running_)
        return Task();
    Task task;
    if(!queue_.empty())
    {
        task = queue_.front();
        queue_.pop_front();
    }
    return task;
}

NAMESPACE_ZL_THREAD_END