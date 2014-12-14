#include "thread/ThreadGroup.h"
#include "thread/Thread.h"
#include "base/StrUtil.h"
NAMESPACE_ZL_THREAD_START

ThreadGroup::ThreadGroup()
{

}

ThreadGroup::~ThreadGroup()
{
    for(size_t i = 0; i < vecThreads_.size(); ++i)
    {
        delete vecThreads_[i];
    }
    vecThreads_.clear();
}

void ThreadGroup::create_thread(std::function<void ()> func, int thread_num/* = 1*/)
{
    LockGuard<Mutex> lock(mutex_);
    for (int i = 0; i < thread_num; ++i)
    {
        std::string thr_name("threadgroup_thread_");
        thr_name += zl::base::toStr(i);
        vecThreads_.push_back(new Thread(func, thr_name));
    }
}

void ThreadGroup::add_thread(Thread *thd)
{
    LockGuard<Mutex> lock(mutex_);
    vecThreads_.push_back(thd);
}

void ThreadGroup::remove_thread(Thread *thd)
{
    LockGuard<Mutex> lock(mutex_);
    std::vector<Thread *>::iterator it = std::find(vecThreads_.begin(), vecThreads_.end(), thd);
    if(it != vecThreads_.end())
    {
        vecThreads_.erase(it);
    }
}

void ThreadGroup::join_all()
{
    LockGuard<Mutex> lock(mutex_);
    for_each(vecThreads_.begin(), vecThreads_.end(), std::bind(&Thread::join, std::placeholders::_1));
}

size_t ThreadGroup::size() const
{
    LockGuard<Mutex> lock(mutex_);
    return vecThreads_.size();
}

NAMESPACE_ZL_THREAD_END