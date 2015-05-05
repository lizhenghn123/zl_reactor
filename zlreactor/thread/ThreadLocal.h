//TODO : 
// ***********************************************************************
// Filename         : ThreadLocal.h
// Author           : LIZHENG
// Created          : 2014-10-07
// Description      : Thread Local Storage(TLS)  似乎不能正常释放（比如在线程中使用该类）
// http://blog.csdn.net/evilswords/article/details/8191230
// http://blog.sina.com.cn/s/blog_a0d7d6bd01017n79.html
// http://www.cs.wustl.edu/~schmidt/PDF/TSS-pattern.pdf
// http://en.cppreference.com/w/cpp/language/storage_duration
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-07
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_THREADLOCAL_H
#define ZL_THREADLOCAL_H
#include "Define.h"
#include "base/NonCopy.h"
#include "thread/Mutex.h"
#include "thread/Condition.h"
#include <stdio.h>
#ifdef OS_WINDOWS
#include <Windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
NAMESPACE_ZL_THREAD_START


#ifdef OS_WINDOWS
template<typename T>
class ThreadLocal : NonCopy
{
public:
    ThreadLocal()
    {
        tlsKey_ = TlsAlloc();
    }

    ~ThreadLocal()
    {
        LPVOID p = TlsGetValue(tlsKey_);
        if(p != NULL)
        {
            T *obj = static_cast<T*>(p);
            delete obj;
        }

        TlsFree(tlsKey_);
    }

public:
    T operator()()
    {
        return *get();
    }

    T& operator*() 
    {
        return *get();
    }

    T* operator->() 
    {
        return get();
    }

    T* get() const
    {
        LPVOID p = TlsGetValue(tlsKey_);
        if(p == NULL)
        {
            T *obj = new T;
            TlsSetValue(tlsKey_, obj);
            return obj;
        }
        else
        {
            return static_cast<T*>(p);
        }
    }

	T* release()
	{
		T *obj = static_cast<T*>(TlsGetValue(tlsKey_));
		TlsSetValue(tlsKey_, NULL);
		return obj;
	}

	void reset(T *p = NULL)
	{
		T *obj = static_cast<T*>(TlsGetValue(tlsKey_));
		delete obj;
		TlsSetValue(tlsKey_, p);
	}

private:
    DWORD tlsKey_;
};

#else
template<typename T>
class ThreadLocal : NonCopy
{
public:
    ThreadLocal()
    {
        pthread_key_create(&tlsKey_, &ThreadLocal::cleanHook);
    }

    ~ThreadLocal()
    {
		T *p = get();
		if(p)
			delete p;
        pthread_key_delete(tlsKey_);
    }

public:
    T operator()()
    {
        return *get();
    }

    T& operator*() 
    {
        return *get();
    }

    T* operator->() 
    {
        return get();
    }

    T* get() const
    {
        T* obj = static_cast<T*>(pthread_getspecific(tlsKey_));
        if (obj == NULL) 
        {
            T* newObj = new T;
            pthread_setspecific(tlsKey_, newObj);
            return newObj;
        }
        return obj;
    }

	T* release()
	{
		T *obj = get();
		pthread_setspecific(tlsKey_, NULL);
		return obj;
	}

	void reset(T *p = NULL)
	{
		T *obj = get();
		delete obj;
		obj = NULL;
		pthread_setspecific(tlsKey_, p);
	}

    static void cleanHook(void *x)
    {
        T* obj = static_cast<T*>(x);
        delete obj;
		obj = NULL;
    }

private:
    pthread_key_t tlsKey_;
};
#endif

NAMESPACE_ZL_THREAD_END
#endif  /* ZL_THREADLOCAL_H */