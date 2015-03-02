#include "thread/ThreadGroup.h"
#include "thread/Thread.h"
#include <assert.h>
NAMESPACE_ZL_THREAD_START

ThreadGroup::ThreadGroup()
{

}

ThreadGroup::~ThreadGroup()
{
    for(auto it=threads_.begin(),end=threads_.end(); it!=end; ++it)
    {
        delete *it;
    }
    threads_.clear();
}

bool ThreadGroup::is_this_thread_in()
{
    Thread::id id = this_thread::get_id();
    LockGuard<Mutex> lock(mutex_);
    for(auto it=threads_.begin(),end=threads_.end(); it!=end; ++it)
    {
        if ((*it)->get_id() == id)
            return true;
    }
    return false;
}

bool ThreadGroup::is_thread_in(Thread* thrd)
{
    if(thrd)
    {
        Thread::id id = thrd->get_id();
        LockGuard<Mutex> lock(mutex_);
        for(auto it=threads_.begin(),end=threads_.end(); it!=end; ++it)
        {
            if ((*it)->get_id() == id)
                return true;
        }
        return false;
    }

    return false;
}

void ThreadGroup::add_thread(Thread *thrd)
{
    if(thrd)
    {
        assert(!is_thread_in(thrd) && "must not add a duplicated thread");
        LockGuard<Mutex> lock(mutex_);
        threads_.push_back(thrd);
    }
}

void ThreadGroup::remove_thread(Thread *thd)
{
    LockGuard<Mutex> lock(mutex_);
    std::vector<Thread *>::iterator it = std::find(threads_.begin(), threads_.end(), thd);
    if(it != threads_.end())
    {
        threads_.erase(it);
    }
}

void ThreadGroup::join_all()
{
    assert(!is_this_thread_in() && "trying joining itself");
    LockGuard<Mutex> lock(mutex_);
    //for_each(threads_.begin(), threads_.end(), std::bind(&Thread::join, std::placeholders::_1));
    for(auto it=threads_.begin(),end=threads_.end(); it!=end; ++it)
    {
        if ((*it)->joinable())
            (*it)->join();
    }
}

size_t ThreadGroup::size() const
{
    LockGuard<Mutex> lock(mutex_);
    return threads_.size();
}

NAMESPACE_ZL_THREAD_END