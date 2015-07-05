// ***********************************************************************
// Filename         : Semaphore.h
// Author           : LIZHENG
// Created          : 2014-06-08
// Description      : 信号量
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SEMAPHORE_H
#define ZL_SEMAPHORE_H
#include "OsDefine.h"
#include "NonCopy.h"
#include <exception>
#ifdef OS_WINDOWS
#include <Windows.h>
#elif defined(OS_LINUX)
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#endif
NAMESPACE_ZL_THREAD_START

class Semaphore : public zl::NonCopy
{
public:
    explicit Semaphore(int initial = 0)
    {
    #ifdef OS_WINDOWS
        sem_ = ::CreateSemaphore(NULL, initial, 0x7fffffff, NULL);
    #elif defined(OS_LINUX)
        sem_init(&sem_, false, initial);
    #endif
    }

    ~Semaphore()
    {
    #ifdef OS_WINDOWS
        ::CloseHandle(sem_);
    #elif defined(OS_LINUX)
        sem_destroy(&sem_);
    #endif
    }

public:
    bool wait()
    {
    #ifdef OS_WINDOWS
        return ::WaitForSingleObject(&sem_, 0) == WAIT_OBJECT_0;
    #elif defined(OS_LINUX)
        return sem_wait(&sem_) == 0;
    #endif
    }

    bool wait(int64_t timeoutMs)
    {
    #ifdef OS_WINDOWS
        return ::WaitForSingleObject(&sem_, timeoutMs) == WAIT_OBJECT_0;
    #elif defined(OS_LINUX)
        struct timespec ts;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int64_t usec = tv.tv_usec + timeoutMs * 1000LL;
        ts.tv_sec = tv.tv_sec + usec / 1000000;
        ts.tv_nsec = (usec % 1000000) * 1000;
        return sem_timedwait(sem, &ts) == 0;
    #endif
    }

    bool try_wait()
    {
    #ifdef OS_WINDOWS
        return ::WaitForSingleObject(sem_, 0) == WAIT_OBJECT_0;
    #elif defined(OS_LINUX)
        return sem_trywait(&sem_) == 0;
    #endif
    }

    bool post(long rc = 1)
    {
    #ifdef OS_WINDOWS
        return ::ReleaseSemaphore(sem_, rc, NULL);
    #elif defined(OS_LINUX)
        return sem_post(&sem_) == 0;
    #endif
    }

private:
#ifdef OS_WINDOWS
    HANDLE sem_;
#elif defined(OS_LINUX)
    sem_t sem_;
#endif
};

NAMESPACE_ZL_THREAD_END
#endif  /* ZL_SEMAPHORE_H */
