// ***********************************************************************
// Filename         : Condition.h
// Author           : LIZHENG
// Created          : 2014-05-16
// Description      : 条件变量在Windows及Linux平台下的封装
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-08-25
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_CONTDITION_H
#define ZL_CONTDITION_H
#include "Define.h"
#include "base/NonCopy.h"
#include "thread/Mutex.h"
NAMESPACE_ZL_THREAD_START

class Condition : public zl::NonCopy
{
public:
    explicit Condition(Mutex& mu) : mutex_(mu)
    {
    #ifdef OS_WINDOWS
        InitializeConditionVariable(&condition_);
    #elif defined(OS_LINUX)
        pthread_cond_init(&condition_, NULL);
    #endif
    }

    ~Condition()
    {
    #ifdef OS_WINDOWS
        //nothing
    #elif defined(OS_LINUX)
        pthread_cond_destroy(&condition_);
    #endif
    }

public:
    void wait()
    {
    #ifdef OS_WINDOWS
        (void)SleepConditionVariableCS(&condition_, mutex_.getMutex(), INFINITE);
    #elif defined(OS_LINUX)
        (void)pthread_cond_wait(&condition_, mutex_.getMutex());
    #endif
    }

    /// returns true only iff time out
    bool timed_wait(int millisecond)
    {
        assert(millisecond >= 0);
    #ifdef OS_WINDOWS
        return SleepConditionVariableCS(&condition_, mutex_.getMutex(), millisecond);
    #elif defined(OS_LINUX)
        struct timespec abstime;
        (void)clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec  += millisecond / 1000;
        abstime.tv_nsec += (millisecond % 1000) * 1000000;  // 1 us = 1000000 ns
        while(abstime.tv_nsec >= 1000000000L)
        {
            ++abstime.tv_sec;
            abstime.tv_nsec %= 1000000000L;
        }
        return ETIMEDOUT == pthread_cond_timedwait(&condition_, mutex_.getMutex(), &abstime);
    #endif
    }

    void notify_one()
    {
    #ifdef OS_WINDOWS
        WakeConditionVariable(&condition_);
    #elif defined(OS_LINUX)
        pthread_cond_signal(&condition_);
    #endif
    }

    void notify_all()
    {
    #ifdef OS_WINDOWS
        WakeAllConditionVariable(&condition_);
    #elif defined(OS_LINUX)
        pthread_cond_broadcast(&condition_);
    #endif
    }

private:
    Mutex&    mutex_;
#ifdef OS_WINDOWS
    CONDITION_VARIABLE condition_;
#elif defined(OS_LINUX)
    pthread_cond_t     condition_;
#endif
};

NAMESPACE_ZL_THREAD_END
#endif /* ZL_CONTDITION_H */
