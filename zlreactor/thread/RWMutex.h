// ***********************************************************************
// Filename         : RWMutex.h
// Author           : LIZHENG
// Created          : 2014-12-14
// Description      : 读写锁在Windows及Linux平台下的封装
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-12-14
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_RWMUTEX_H
#define ZL_RWMUTEX_H
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

class RWMutex
{
    DISALLOW_COPY_AND_ASSIGN(RWMutex);
public:
    RWMutex()
    {
    #ifdef OS_WINDOWS
        ::InitializeSRWLock(&rwlock_);
    #elif defined(OS_LINUX)
        pthread_rwlock_init(&rwlock_, NULL);
    #endif
    }

    ~RWMutex()
    {
    #ifdef OS_WINDOWS
        //nothing
    #elif defined(OS_LINUX)
        pthread_rwlock_destroy(&rwlock_);
    #endif
    }

public:

    bool readLock()
    {
    #ifdef OS_WINDOWS
        ::AcquireSRWLockShared(&rwlock_);
        return true;
    #elif defined(OS_LINUX)
        return pthread_rwlock_rdlock(&rwlock_) == 0;
    #endif
    }

    bool readUnLock()
    {
    #ifdef OS_WINDOWS
        ::ReleaseSRWLockShared(&rwlock_);
        return true;
    #elif defined(OS_LINUX)
        return pthread_rwlock_unlock(&rwlock_) == 0;
    #endif
    }

    bool writeLock()
    {
    #ifdef OS_WINDOWS
        ::AcquireSRWLockExclusive(&rwlock_);
        return true;
    #elif defined(OS_LINUX)
        return pthread_rwlock_wrlock(&rwlock_) == 0;
    #endif
    }

    bool writeUnLock()
    {
    #ifdef OS_WINDOWS
        ::ReleaseSRWLockExclusive(&rwlock_);
        return true;
    #elif defined(OS_LINUX)
        return pthread_rwlock_unlock(&rwlock_) == 0;
    #endif
    }

    bool tryReadLock()
    {
    #ifdef OS_WINDOWS
        return ::TryAcquireSRWLockShared(&rwlock_) == TRUE;
    #elif defined(OS_LINUX)
        return pthread_rwlock_tryrdlock(&rwlock_) == 0;
    #endif
    }

    bool tryWriteLock()
    {
    #ifdef OS_WINDOWS
        return ::TryAcquireSRWLockShared(&rwlock_) == TRUE;
    #elif defined(OS_LINUX)
        return pthread_rwlock_trywrlock(&rwlock_) == 0;
    #endif
    }

private:
#ifdef OS_WINDOWS
    SRWLOCK          rwlock_;  //not support Windows XP
#elif defined(OS_LINUX)
    pthread_rwlock_t rwlock_;
#endif
};


class RWMutexReadLockGuard
{
    DISALLOW_COPY_AND_ASSIGN(RWMutexReadLockGuard);
public:
    explicit RWMutexReadLockGuard(RWMutex& mutex) : mutex_(mutex)
    {
        mutex_.readLock();
    }
    ~RWMutexReadLockGuard()
    {
        mutex_.readUnLock();
    }
private:
    RWMutex& mutex_;
};

class RWMutexReadTryLockGuard
{
    DISALLOW_COPY_AND_ASSIGN(RWMutexReadTryLockGuard);
public:
    explicit RWMutexReadTryLockGuard(RWMutex& mutex) : mutex_(mutex)
    {
        isLocked_ = mutex_.tryReadLock();
    }
    ~RWMutexReadTryLockGuard()
    {
        if(isLocked_)
            mutex_.readUnLock();
    }
    bool IsLocked()
    {
        return isLocked_;
    }

private:
    bool  isLocked_;
    RWMutex& mutex_;
};

class RWMutexWriteLockGuard
{
    DISALLOW_COPY_AND_ASSIGN(RWMutexWriteLockGuard);
public:
    explicit RWMutexWriteLockGuard(RWMutex& mutex) : mutex_(mutex)
    {
        mutex_.writeLock();
    }
    ~RWMutexWriteLockGuard()
    {
        mutex_.writeUnLock();
    }
private:
    RWMutex& mutex_;
};

class RWMutexWriteTryLockGuard
{
    DISALLOW_COPY_AND_ASSIGN(RWMutexWriteTryLockGuard);
public:
    explicit RWMutexWriteTryLockGuard(RWMutex& mutex) : mutex_(mutex)
    {
        isLocked_ = mutex_.tryWriteLock();
    }
    ~RWMutexWriteTryLockGuard()
    {
        if(isLocked_)
            mutex_.writeUnLock();
    }
    bool IsLocked()
    {
        return isLocked_;
    }

private:
    bool  isLocked_;
    RWMutex& mutex_;
};

NAMESPACE_ZL_THREAD_END
#endif  /* ZL_RWMUTEX_H */