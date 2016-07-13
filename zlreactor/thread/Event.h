// ***********************************************************************
// Filename         : Event.h
// Author           : LIZHENG
// Description      : 同步原语，可用于一个线程在某一事件发生时通知其他(多个)线程
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_EVENT_H
#define ZL_EVENT_H
#include "zlreactor/Define.h"
#include "zlreactor/base/NonCopy.h"
#include "zlreactor/thread/Mutex.h"
#include "zlreactor/thread/Condition.h"
NAMESPACE_ZL_THREAD_START

class Event : public zl::NonCopy
{
public:
    /// signal为false表示除非调用set, 否则当前事件状态就是non-signal的;
    /// autoreset为true时表示该事件在wait成功返回时会自动重置;
    explicit Event(bool signal = false, bool autoreset = true)
        : mutex_()
        , condition_(mutex_)
        , signaled_(signal)
        , autoReset_(autoreset)
    {

    }

    ~Event()
    {

    }

public:
	/// block until signaled_ == true
    void wait()
    {
        LockGuard<Mutex> lock(mutex_);
        while (!signaled_)
        {
            condition_.wait();
        }
        
        if(autoReset_)
        {
            signaled_ = false;
        }
    }

    bool timed_wait(int millisecond)
    {
        LockGuard<Mutex> lock(mutex_);
        if (!signaled_)
        {
            condition_.timed_wait(millisecond);
        }
        
        if (!signaled_)
        {
            return false;
        }

        if(autoReset_)
        {
            signaled_ = false;
        }
        return true;
    }

    bool try_wait()
    {
        return timed_wait(0);    
    }
    
    void set()
    {
        LockGuard<Mutex> lock(mutex_);
        signaled_ = true;
        condition_.notify_all();
    }

    void reset()
    {
        LockGuard<Mutex> lock(mutex_);
        signaled_ = false;
    }

    bool autoReset() const
	{
		return autoReset_;
	}

private:
    zl::thread::Mutex     mutex_;
    zl::thread::Condition condition_;
    bool                  signaled_;
    const bool            autoReset_;
};

NAMESPACE_ZL_THREAD_END
#endif /* ZL_CONTDITION_H */
