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
#ifdef OS_LINUX
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#elif defined(OS_WINDOWS)
#define OS_WINDOWS
#include <Windows.h>
#endif

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
        SleepConditionVariableCS(&condition_, mutex_.getMutex(), INFINITE);
    #elif defined(OS_LINUX)
        pthread_cond_wait(&condition_, mutex_.getMutex());
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
