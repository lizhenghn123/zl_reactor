// ***********************************************************************
// Filename         : Mutex.h
// Author           : LIZHENG
// Created          : 2014-05-16
// Description      : 互斥锁在Windows及Linux平台下的封装
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-06-24
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_MUTEX_H
#define ZL_MUTEX_H
#include "Define.h"
#ifdef OS_WINDOWS
#include <Windows.h>
#elif defined(OS_LINUX)
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#endif

NAMESPACE_ZL_THREAD_START

#ifdef NDEBUG
#define THREAD_CHECK(func)  {  int errnum = (func);       \
                               if(errnum != 0)            \
                                   fprintf(stderr, "%s:%d : [%d]\n", __FILE__, __LINE__, errnum);  \
                            }
#else
#define THREAD_CHECK(func)  {  int errnum = (func);       \
                               if(errnum != 0)            \
                               {                          \
                                   fprintf(stderr, "%s:%d : [%d]\n", __FILE__, __LINE__, errnum); \
                                   assert(errnum == 0);   \
                               }                          \
                            }
#endif

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
};

class SpinMutex
{
public:
    SpinMutex()
    {
        lock_ = 0;
    }

    ~SpinMutex()
    {
    } 

    void lock()
    {
        while(__sync_lock_test_and_set(&lock_, 1))
        {
        }
    }

    void unlock()
    {
        __sync_lock_release(&lock_);
    }

private:
    volatile int lock_;
};

class Mutex
{
    DISALLOW_COPY_AND_ASSIGN(Mutex);
public:
    Mutex()
    {
    #ifdef OS_WINDOWS
        InitializeCriticalSection(&mutex_);
    #elif defined(OS_LINUX)
        pthread_mutex_init(&mutex_, NULL);
    #endif
    }

    ~Mutex()
    {
    #if defined(OS_WINDOWS)
        DeleteCriticalSection(&mutex_);
    #else
        pthread_mutex_destroy(&mutex_);
    #endif
    }

public:
    void lock()
    {
    #ifdef OS_WINDOWS
        EnterCriticalSection(&mutex_);
    #elif defined(OS_LINUX)
        THREAD_CHECK(pthread_mutex_lock(&mutex_));
    #endif
    }

    bool try_lock()
    {
    #ifdef OS_WINDOWS
        #if(_WIN32_WINNT >= 0x0400)
        return TryEnterCriticalSection(&mutex_) != 0;
        #else
        return false;
        #endif
    #elif defined(OS_LINUX)
        return pthread_mutex_trylock(&mutex_) == 0;
    #endif
    }

    void unlock()
    {
    #ifdef OS_WINDOWS
        LeaveCriticalSection(&mutex_);
    #elif defined(OS_LINUX)
        THREAD_CHECK(pthread_mutex_unlock(&mutex_));
    #endif
    }

#ifdef OS_WINDOWS
    CRITICAL_SECTION* getMutex()
#elif defined(OS_LINUX)
    pthread_mutex_t* getMutex()
#endif
    {
        return &mutex_;
    }

private:
#ifdef OS_WINDOWS
    mutable CRITICAL_SECTION mutex_;
#elif defined(OS_LINUX)
    pthread_mutex_t mutex_;
#endif
};


class RecursiveMutex
{
    DISALLOW_COPY_AND_ASSIGN(RecursiveMutex);
public:
    RecursiveMutex()
    {
    #if defined(OS_WINDOWS)
        InitializeCriticalSection(&mutex_);
    #else
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mutex_, &attr);
    #endif
    }

    ~RecursiveMutex()
    {
    #if defined(OS_WINDOWS)
        DeleteCriticalSection(&mutex_);
    #else
        pthread_mutex_destroy(&mutex_);
    #endif
    }

    void lock()
    {
    #if defined(OS_WINDOWS)
        EnterCriticalSection(&mutex_);
    #else
        THREAD_CHECK(pthread_mutex_lock(&mutex_));
    #endif
    }

    bool try_lock()
    {
    #if defined(OS_WINDOWS)
        return TryEnterCriticalSection(&mutex_) != 0;
    #else
        return pthread_mutex_trylock(&mutex_) == 0;
    #endif
    }

    void unlock()
    {
    #if defined(OS_WINDOWS)
        LeaveCriticalSection(&mutex_);
    #else
        THREAD_CHECK(pthread_mutex_unlock(&mutex_));
    #endif
    }

#ifdef OS_WINDOWS
    CRITICAL_SECTION* getMutex()
#elif defined(OS_LINUX)
    pthread_mutex_t* getMutex()
#endif
    {
        return &mutex_;
    }

private:
#ifdef OS_WINDOWS
    mutable CRITICAL_SECTION mutex_;
#elif defined(OS_LINUX)
    pthread_mutex_t mutex_;
#endif
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
    MutexType& mutex_;
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
    MutexType& mutex_;
};

NAMESPACE_ZL_THREAD_END

#endif  /* ZL_MUTEX_H */
