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

class RWMutex
{
    DISALLOW_COPY_AND_ASSIGN(RWMutex);
public:
    RWMutex()
    {
        init_();
    }
    ~RWMutex()
    {
        close_();
    }
public:
    bool readLock()
    {
#ifdef	OS_WINDOWS
        ::AcquireSRWLockShared(&rwlock_);
        return true;
#elif defined(OS_LINUX)
        return pthread_rwlock_rdlock(&rwlock_) == 0;
#endif
    }
    bool readUnLock()
    {
#ifdef	OS_WINDOWS
        ::ReleaseSRWLockShared(&rwlock_);
        return true;
#elif defined(OS_LINUX)
        return pthread_rwlock_unlock(&rwlock_) == 0;
#endif
    }
    bool writeLock()
    {
#ifdef	OS_WINDOWS
        ::AcquireSRWLockExclusive(&rwlock_);
        return true;
#elif defined(OS_LINUX)
        return pthread_rwlock_wrlock(&rwlock_) == 0;
#endif
    }
    bool writeUnLock()
    {
#ifdef	OS_WINDOWS
        ::ReleaseSRWLockExclusive(&rwlock_);
        return true;
#elif defined(OS_LINUX)
        return pthread_rwlock_unlock(&rwlock_) == 0;
#endif
    }
    bool tryReadLock()
    {
#ifdef	OS_WINDOWS
        return ::TryAcquireSRWLockShared(&rwlock_) == TRUE;
#elif defined(OS_LINUX)
        return pthread_rwlock_tryrdlock(&rwlock_) == 0;
#endif
    }
    bool tryWriteLock()
    {
#ifdef	OS_WINDOWS
        return ::TryAcquireSRWLockShared(&rwlock_) == TRUE;
#elif defined(OS_LINUX)
        return pthread_rwlock_trywrlock(&rwlock_) == 0;
#endif
    }

private:
    void init_()
    {
#ifdef	OS_WINDOWS
        ::InitializeSRWLock(&rwlock_);
#elif defined(OS_LINUX)
        pthread_rwlock_init(&rwlock_, NULL);
#endif
    }
    void close_()
    {
#ifdef	OS_WINDOWS
        //nothing
#elif defined(OS_LINUX)
        pthread_rwlock_destroy(&rwlock_);
#endif
    }

private:
#ifdef	OS_WINDOWS
    SRWLOCK          rwlock_;  //not support Windows XP
#elif defined(OS_LINUX)
    pthread_rwlock_t rwlock_;
#endif
};


class MutexLocker
{
    DISALLOW_COPY_AND_ASSIGN(MutexLocker);
public:
    explicit MutexLocker(Mutex& mutex) : mutex_(mutex)
    {
        mutex_.lock();
    }
    ~MutexLocker()
    {
        mutex_.unlock();
    }
private:
    Mutex& mutex_;
};

class MutexTryLocker
{
    DISALLOW_COPY_AND_ASSIGN(MutexTryLocker);
public:
    explicit MutexTryLocker(Mutex& mutex) : mutex_(mutex)
    {
        isLocked_ = mutex_.try_lock();
    }
    ~MutexTryLocker()
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
    Mutex& mutex_;
};

class RWMutexReadLocker
{
    DISALLOW_COPY_AND_ASSIGN(RWMutexReadLocker);
public:
    explicit RWMutexReadLocker(RWMutex& mutex) : mutex_(mutex)
    {
        mutex_.readLock();
    }
    ~RWMutexReadLocker()
    {
        mutex_.readUnLock();
    }
private:
    RWMutex& mutex_;
};

class RWMutexReadTryLocker
{
    DISALLOW_COPY_AND_ASSIGN(RWMutexReadTryLocker);
public:
    explicit RWMutexReadTryLocker(RWMutex& mutex) : mutex_(mutex)
    {
        isLocked_ = mutex_.tryReadLock();
    }
    ~RWMutexReadTryLocker()
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

class RWMutexWriteLocker
{
    DISALLOW_COPY_AND_ASSIGN(RWMutexWriteLocker);
public:
    explicit RWMutexWriteLocker(RWMutex& mutex) : mutex_(mutex)
    {
        mutex_.writeLock();
    }
    ~RWMutexWriteLocker()
    {
        mutex_.writeUnLock();
    }
private:
    RWMutex& mutex_;
};

class RWMutexWriteTryLocker
{
    DISALLOW_COPY_AND_ASSIGN(RWMutexWriteTryLocker);
public:
    explicit RWMutexWriteTryLocker(RWMutex& mutex) : mutex_(mutex)
    {
        isLocked_ = mutex_.tryWriteLock();
    }
    ~RWMutexWriteTryLocker()
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

#endif  /* ZL_MUTEX_H */