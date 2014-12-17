// ***********************************************************************
// Filename         : FastMutex.h
// Author           : LIZHENG
// Created          : 2014-12-17
// Description      : Refer : http://tinythreadpp.bitsnbites.eu/
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-12-17
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_FASTMUTEX_H
#define ZL_FASTMUTEX_H
#include "Define.h"
#ifdef OS_WINDOWS
#include <Windows.h>
#elif defined(OS_LINUX)
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#endif

// Check if we can support the assembly language level implementation,
// otherwise revert to the system API)
#if (defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))) || \
    (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))) || \
    (defined(__GNUC__) && (defined(__ppc__)))
#define FAST_MUTEX_ASM
#endif

NAMESPACE_ZL_THREAD_START

class FastMutex
{
    DISALLOW_COPY_AND_ASSIGN(FastMutex);
public:
    FastMutex()
    {
    #if defined(FAST_MUTEX_ASM)
        lock_ = 0;
    #elif defined(OS_WINDOWS)
        InitializeCriticalSection(&mutex_);
    #elif defined(OS_LINUX)
        pthread_mutex_init(&mutex_, NULL);
    #endif
    }

    ~FastMutex()
    {
    #if defined(FAST_MUTEX_ASM)
    #elif defined(OS_WINDOWS)
        DeleteCriticalSection(&mutex_);
    #elif defined(OS_LINUX)
        pthread_mutex_destroy(&mutex_);
    #endif
    }

public:
    void lock()
    {
    #if defined(FAST_MUTEX_ASM)
        bool gotLock;
        do
        {
            gotLock = try_lock();
            if(!gotLock)
            {
            #if defined(OS_WINDOWS) 
                Sleep(0);
            #elif defined(OS_LINUX)
                sched_yield();
            #endif
            }
        } while(!gotLock);
    #elif defined(OS_WINDOWS)
        EnterCriticalSection(&mutex_);
    #elif defined(OS_LINUX)
        pthread_mutex_lock(&mutex_);
    #endif
    }

    bool try_lock()
    {
    #if defined(FAST_MUTEX_ASM)
        int oldLock;
        #if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
            asm volatile (
                "movl $1,%%eax\n\t"
                "xchg %%eax,%0\n\t"
                "movl %%eax,%1\n\t"
                : "=m" (lock_), "=m" (oldLock)
                :
            : "%eax", "memory"
                );
        #elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
                int *ptrLock = &lock_;
                __asm {
                    mov eax,1
                        mov ecx,ptrLock
                        xchg eax,[ecx]
                    mov oldLock,eax
                }
        #elif defined(__GNUC__) && (defined(__ppc__))
                int newLock = 1;
                asm volatile (
                    "\n1:\n\t"
                    "lwarx  %0,0,%1\n\t"
                    "cmpwi  0,%0,0\n\t"
                    "bne-   2f\n\t"
                    "stwcx. %2,0,%1\n\t"
                    "bne-   1b\n\t"
                    "isync\n"
                    "2:\n\t"
                    : "=&r" (oldLock)
                    : "r" (&lock_), "r" (newLock)
                    : "cr0", "memory"
                    );
        #endif
        return (oldLock == 0);
    #elif defined(OS_WINDOWS)
        return TryEnterCriticalSection(&mHandle) ? true : false;
    #elif defined(OS_LINUX)
        return (pthread_mutex_trylock(&mHandle) == 0);
    #endif
    }

    void unlock()
    {
    #if defined(FAST_MUTEX_ASM)
        #if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
            asm volatile (
                "movl $0,%%eax\n\t"
                "xchg %%eax,%0\n\t"
                : "=m" (lock_)
                :
                : "%eax", "memory"
            );
        #elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
            int *ptrLock = &lock_;
            __asm {
                mov eax,0
                    mov ecx,ptrLock
                    xchg eax,[ecx]
            }
        #elif defined(__GNUC__) && (defined(__ppc__))
            asm volatile (
                "sync\n\t"  // Replace with lwsync where possible?
                : : : "memory"
            );
            lock_ = 0;
        #endif
    #elif defined(OS_WINDOWS)
            LeaveCriticalSection(&mutex_);
    #elif defined(OS_LINUX)
            pthread_mutex_unlock(&mutex_);
    #endif
    }

#if defined(FAST_MUTEX_ASM)

#elif defined(OS_WINDOWS)
    CRITICAL_SECTION* getMutex()
    {
        return &mutex_;
    }
#elif defined(OS_LINUX)
    pthread_mutex_t* getMutex()
    {
        return &mutex_;
    }
#endif

private:
#if defined(FAST_MUTEX_ASM)
    int lock_;
#elif  defined(OS_WINDOWS)
    mutable CRITICAL_SECTION mutex_;
#elif defined(OS_LINUX)
    pthread_mutex_t mutex_;
#endif
};

NAMESPACE_ZL_THREAD_END
#endif  /* ZL_MUTEX_H */
