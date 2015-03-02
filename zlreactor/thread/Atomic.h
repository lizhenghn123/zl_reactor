// ***********************************************************************
// Filename         : Atomic.h
// Author           : LIZHENG
// Created          : 2014-08-21
// Description      : 原子操作计数器, 不建议在Windows平台下使用（未测试）
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-08-21
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_ATOMIC_H
#define ZL_ATOMIC_H
#include "Define.h"
#include "thread/Mutex.h"
#ifdef OS_LINUX
#include <pthread.h>
#include <unistd.h>  //for usleep
#include <assert.h>
#include <stdlib.h>
#include <sched.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <linux/types.h>
#include <sys/time.h>

#define ATOMIC_ADD(ptr, v)            __sync_add_and_fetch(ptr, v)
#define ATOMIC_SUB(ptr, v)            __sync_sub_and_fetch(ptr, v)
#define ATOMIC_ADD_AND_FETCH(ptr, v)  __sync_add_and_fetch(ptr, v)
#define ATOMIC_SUB_AND_FETCH(ptr, v)  __sync_sub_and_fetch(ptr, v)
#define ATOMIC_FETCH_AND_ADD(ptr, v)  __sync_fetch_and_add(ptr, v)
#define ATOMIC_FETCH_AND_SUB(ptr, v)  __sync_fetch_and_sub(ptr, v)
#define ATOMIC_FETCH(ptr)             __sync_add_and_fetch(ptr, 0)
//#define ATOMIC_SET(ptr, v)            __sync_bool_compare_and_swap(ptr, *(ptr), v)
#define ATOMIC_SET(ptr, v)            __sync_val_compare_and_swap(ptr, *(ptr), v) 

#elif defined(OS_WINDOWS)
#include <Windows.h>
#define ATOMIC_ADD(ptr, v)            ::InterlockedIncrement((long*)ptr)
#define ATOMIC_SUB(ptr, v)            ::InterlockedDecrement((long*)ptr)
//#define ATOMIC_ADD_AND_FETCH(ptr, v)  ::InterlockedExchangeAdd(ptr, v)
//#define ATOMIC_SUB_AND_FETCH(ptr, v)  ::InterlockedExchangeAdd(ptr, -v)
#define ATOMIC_FETCH_AND_ADD(ptr, v)  ::InterlockedExchangeAdd((long*)ptr, v)    /*返回加之前的值*/
#define ATOMIC_FETCH_AND_SUB(ptr, v)  ::InterlockedExchangeAdd((long*)ptr, -v)
#define ATOMIC_FETCH(ptr)             ::InterlockedExchangeAdd((long*)ptr, 0)
#endif

NAMESPACE_ZL_THREAD_START

template <typename T>
class Atomic
{
public:
    typedef  volatile T atomic_t;
public:
    Atomic()
    {
    #ifdef OS_LINUX
        ATOMIC_SET(&atomic_, 0);
    #else
        LockGuard<Mutex> lock(mutex_);
        atomic_ = 0;
    #endif
    }
public:
    inline atomic_t inc(T n = 1)
    {
        return ATOMIC_ADD(&atomic_, n);
    }
    inline atomic_t incAndFetch(T n = 1)
    {
    #ifdef OS_LINUX
        return ATOMIC_ADD_AND_FETCH(&atomic_, n);
    #else
        LockGuard<Mutex> lock(mutex_);
        atomic_ += n;
        return atomic_;
    #endif
    }
    inline atomic_t fetchAndInc(T n = 1)
    {
        return ATOMIC_FETCH_AND_ADD(&atomic_, n);
    }
    inline atomic_t dec(T n = 1)
    {
        return ATOMIC_SUB(&atomic_, n);
    }
    inline atomic_t decAndFetch(T n = 1)
    {
    #ifdef OS_LINUX
        return ATOMIC_SUB_AND_FETCH(&atomic_, n);
    #else
        LockGuard<Mutex> lock(mutex_);
        atomic_ -= n;
        return atomic_;
    #endif
    }
    inline atomic_t fetchAndDec(T n = 1)
    {
        return ATOMIC_FETCH_AND_SUB(&atomic_, n);
    }
    inline atomic_t value()
    {
        return ATOMIC_FETCH(&atomic_);
        return ATOMIC_FETCH_AND_ADD(&atomic_, 0);
    }
public:
    atomic_t operator++()
    {
        return inc(1);
    }
    atomic_t operator--()
    {
        return dec(1);
    }
    atomic_t operator++(int)
    {
        return fetchAndInc(1);
    }
    atomic_t operator--(int)
    {
        return fetchAndDec(1);
    }
    atomic_t operator+=(T num)
    {
        return incAndFetch(num);
    }
    atomic_t operator-=(T num)
    {
        return decAndFetch(num);
    }
    bool operator==(T value)
    {
        return (value() == value);
    }
    operator atomic_t() const
    {
        return atomic_;
    }

private:
    atomic_t atomic_;
#ifdef OS_WINDOWS
    Mutex    mutex_;
#endif
};

template<>
class Atomic<bool>
{
public:
    Atomic()
    {
    #ifdef OS_LINUX
        ATOMIC_SET(&atomic_, 0);
    #else
        LockGuard<Mutex> lock(mutex_);
        atomic_ = 0;
    #endif
    }

    Atomic(bool value)
    {
    #ifdef OS_LINUX
        ATOMIC_SET(&atomic_, value ? 1 : 0);
    #else
        LockGuard<Mutex> lock(mutex_);
        atomic_ = value;
    #endif
    }

    Atomic& operator=(bool value)
    {
    #ifdef OS_LINUX
        ATOMIC_SET(&atomic_, value ? 1 : 0);
    #else
        LockGuard<Mutex> lock(mutex_);
        atomic_ = value;
    #endif
        return *this;
    }

    bool clear()     // set false
    {
    #ifdef OS_LINUX
         return ATOMIC_SET(&atomic_, 0);
    #else
         LockGuard<Mutex> lock(mutex_);
         int oldvalue = atomic_;
         atomic_ = 0;
         return oldvalue;
    #endif
    }

    bool test_and_set()  //set true and return old value
    {  
    #ifdef OS_LINUX
        return ATOMIC_SET(&atomic_, 1);
    #else
        LockGuard<Mutex> lock(mutex_);
        int oldvalue = atomic_;
        atomic_ = 1;
        return oldvalue;
    #endif
    }

    operator bool()
    {
    #ifdef OS_LINUX
        return ATOMIC_FETCH(&atomic_);
    #else
        LockGuard<Mutex> lock(mutex_);
        return atomic_;
    #endif
    }

private:
    volatile int  atomic_;
#ifdef OS_WINDOWS
    Mutex    mutex_;
#endif
};

NAMESPACE_ZL_THREAD_END
#endif /* ZL_ATOMIC_H */
