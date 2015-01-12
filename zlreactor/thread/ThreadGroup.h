// ***********************************************************************
// Filename         : ThreadGroup.h
// Author           : LIZHENG
// Created          : 2014-10-13
// Description      :
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-13
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_THREADGROU_H
#define ZL_THREADGROU_H
#include "Define.h"
#include "base/NonCopy.h"
#include "thread/Mutex.h"
NAMESPACE_ZL_THREAD_START
class Thread;

class ThreadGroup : NonCopy
{
public:
    ThreadGroup();
    ~ThreadGroup();

public:
    void create_thread(std::function<void ()> func, int thread_num = 1);
    void add_thread(Thread *thd);
    void remove_thread(Thread *thd);
    void join_all();
    size_t size() const;

private:
    mutable Mutex         mutex_;
    std::vector<Thread *>  vecThreads_;
};

NAMESPACE_ZL_THREAD_END
#endif  /* ZL_THREADGROU_H */