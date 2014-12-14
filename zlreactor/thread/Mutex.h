// ***********************************************************************
// Filename         : Mutex.h
// Author           : LIZHENG
// Created          : 2014-05-16
// Description      : 互斥锁在Windows及Linux平台下的封装
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-08-25
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_MUTEX_H
#define ZL_MUTEX_H
#include "Define.h"
#include <exception>
#ifdef OS_WINDOWS
#include <Windows.h>
#elif defined(OS_LINUX)
#define OS_LINUX
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#endif

NAMESPACE_ZL_THREAD_START

class Mutex
{
    DISALLOW_COPY_AND_ASSIGN(Mutex);
public:
    Mutex()
    {
        init_();
    }
    ~Mutex()
    {
        close_();
    }

public:
    void lock()
    {
#ifdef	OS_WINDOWS
        EnterCriticalSection(&mutex_);
#elif defined(OS_LINUX)
        if(pthread_mutex_lock(&mutex_) != 0)
        {
            throw std::exception();
        }
#endif
    }

    bool try_lock() //const
    {
#ifdef	OS_WINDOWS
#if(_WIN32_WINNT >= 0x0400)
        return TryEnterCriticalSection(&mutex_) != 0;
#else
        return false;
#endif
#elif defined(OS_LINUX)
        int ret = pthread_mutex_trylock(&mutex_);
        if(ret != 0)
        {
            printf("pthread_mutex_trylock fail,code = %d\n", ret);
            return false;
        }
        return true;
#endif
    }

    void unlock()
    {
#ifdef	OS_WINDOWS
        LeaveCriticalSection(&mutex_);
#elif defined(OS_LINUX)
        if(pthread_mutex_unlock(&mutex_) != 0)
        {
            printf("pthread_mutex_unlock\n");
        }
#endif
    }

#ifdef	OS_WINDOWS
    CRITICAL_SECTION	*GetMutex()
#elif defined(OS_LINUX)
    pthread_mutex_t	*getMutex()
#endif
    {
        return &mutex_;
    }

private:
    void init_()
    {
#ifdef	OS_WINDOWS
#if (_WIN32_WINNT >= 0x0403)
        if(!InitializeCriticalSectionAndSpinCount(&mutex_, 0x80000400))
#endif /* _WIN32_WINNT >= 0x0403 */
            InitializeCriticalSection(&mutex_);
#elif defined(OS_LINUX)
        pthread_mutex_init(&mutex_, NULL);
#endif
    }

    void close_()
    {
#ifdef	OS_WINDOWS
        DeleteCriticalSection(&mutex_);
#elif defined(OS_LINUX)
        pthread_mutex_destroy(&mutex_);
#endif
    }

private:
#ifdef	OS_WINDOWS
    mutable CRITICAL_SECTION mutex_;
#elif defined(OS_LINUX)
    pthread_mutex_t mutex_;
#endif
};

class NullMutex
{
    DISALLOW_COPY_AND_ASSIGN(NullMutex);
public:
    NullMutex()
    {
    }
    ~NullMutex()
    {
    }

public:
    void lock()
    {
    }

    bool try_lock()
    {
        return true;
    }

    void unlock()
    {
    }

#ifdef	OS_WINDOWS
    CRITICAL_SECTION	*GetMutex()
#elif defined(OS_LINUX)
    pthread_mutex_t	*getMutex()
#endif
    {
        return NULL;
    }
};

template <class MutexType>
class LockGuard
{
    DISALLOW_COPY_AND_ASSIGN(LockGuard);
public:
    explicit LockGuard(MutexType& mutex) : mutex_(mutex)
    {
        mutex_.lock();
    }
    ~LockGuard()
    {
        mutex_.unlock();
    }
private:
    mutable MutexType& mutex_;
};

template <class MutexType>
class MutexTryLockGuard
{
    DISALLOW_COPY_AND_ASSIGN(MutexTryLockGuard);
public:
    explicit MutexTryLockGuard(MutexType& mutex) : mutex_(mutex)
    {
        isLocked_ = mutex_.try_lock();
    }
    ~MutexTryLockGuard()
    {
        if(isLocked_)
            mutex_.unlock();
    }
    bool IsLocked()
    {
        return isLocked_;
    }

private:
    bool  isLocked_;
    mutable MutexType& mutex_;
};

NAMESPACE_ZL_THREAD_END

#endif  /* ZL_MUTEX_H */