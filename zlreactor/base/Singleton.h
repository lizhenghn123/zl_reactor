// ***********************************************************************
// Filename         : Singleton.h
// Author           : LIZHENG
// Description      : 线程安全的单例模板类.可用于多线程中，可手动或自动释放单例实例
//                    使用方式：
//                    class TestSingleton : public SingleTon<TestSingleton>
//                    {
//                         DECLARE_SINGLETON_CLASS(type);
//                    }
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SINGLETON_H
#define ZL_SINGLETON_H
#include "Define.h"
#include "thread/Mutex.h"
NAMESPACE_ZL_START

#define DECLARE_SINGLETON_CLASS(type)  friend class zl::Singleton< type >

template <class T>
class Singleton
{
public:
    static T* getInstancePtr()
    {
        if(0 == proxy_.instance_)
        {
            createInstance();
        }
        return proxy_.instance_;
    }

    static T& getInstanceRef()
    {
        if(0 == proxy_.instance_)
        {
            createInstance();
        }
        return *(proxy_.instance_);
    }

    static T* createInstance()
    {
        return proxy_.createInstance();
    }

    static void deleteInstance()
    {
        proxy_.deleteInstance();
    }

private:
    struct Proxy
    {
        Proxy() : instance_(0)
        {
        }

        ~Proxy()
        {
            if(instance_)
            {
                delete instance_;
                instance_ = 0;
            }
        }

        T* createInstance()
        {
            T *p = instance_;
            if(p == 0)
            {
                zl::thread::LockGuard<zl::thread::Mutex> guard(lock_);
                if((p = instance_) ==0)
                {
                    instance_ = p = new T;
                }
            }
            return instance_;
        }

        void deleteInstance()
        {
            if(proxy_.instance_)
            {
                delete proxy_.instance_;
                proxy_.instance_ = 0;
            }
        }
        T *instance_;
        zl::thread::Mutex lock_;
    };
protected:
    Singleton()  {	}
    ~Singleton() {	}
private:
    static Proxy proxy_;
};

template < class T >
typename Singleton<T>::Proxy Singleton<T>::proxy_;


NAMESPACE_ZL_END
#endif /* ZL_SINGLETON_H */
