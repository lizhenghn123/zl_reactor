// ***********************************************************************
// Filename         : ThreadGroup.h
// Author           : LIZHENG
// Created          : 2014-10-13
// Description      :
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-03-01
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_THREADGROU_H
#define ZL_THREADGROU_H
#include "Define.h"
#include "base/NonCopy.h"
#include "thread/Thread.h"
#include "thread/Mutex.h"
NAMESPACE_ZL_THREAD_START
class Thread;

class ThreadGroup : NonCopy
{
public:
    ThreadGroup();
    ~ThreadGroup();

public:
    template<typename F>
    Thread* create_thread(F threadfunc, const std::string& thrd_name="")
    {
        Thread *trd = new Thread(threadfunc, thrd_name);
        LockGuard<Mutex> lock(mutex_);
        threads_.push_back(trd);
        return trd;
    }

    void add_thread(Thread *thd);
    void remove_thread(Thread *thd);
    void join_all();
    size_t size() const;

private:
    bool is_this_thread_in();
    bool is_thread_in(Thread* thrd);

private:
    mutable Mutex          mutex_;
    std::vector<Thread *>  threads_;
};

NAMESPACE_ZL_THREAD_END
#endif  /* ZL_THREADGROU_H */