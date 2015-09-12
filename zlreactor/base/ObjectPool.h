// ***********************************************************************
// Filename         : ObjectPool.h
// Author           : LIZHENG
// Created          : 
// Description      : 简单的对象池实现（支持单/多线程配置、固定类型分配总数设置）
//
// Last Modified By : LIZHENG
// Last Modified On : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_OBJECTPOOL_H
#define ZL_OBJECTPOOL_H
#include <assert.h>
#include <list>
#include <vector>
#include "thread/Mutex.h"
namespace zl{ namespace base {

template <typename T, class LockType = zl::thread::Mutex>
class ObjectPool
{
    enum { size_per_alloc = 16 };
public:
    explicit ObjectPool(int preAllocNum = 4, int maxAllocNum = 0)
    {
        totalAllocNum_ = 0;
        maxAllocNum_ = maxAllocNum;
        pre_alloc(preAllocNum <= 0 ? size_per_alloc : preAllocNum);
    }

    ~ObjectPool()
    {
        for(size_t i = 0; i < chunks_.size(); i++)
        {
            delete[] chunks_[i];
        }
    }

public:
    T* alloc()
    {
        zl::thread::LockGuard<LockType> lock(mutex_);
        if(pools_.empty() && !pre_alloc(size_per_alloc))
        {
            return NULL;
        }

        T *t = pools_.front();
        pools_.pop_front();
        return t;
    }

    void free(T *t)
    {
        zl::thread::LockGuard<LockType> lock(mutex_);
        pools_.push_front(t);
    }

    int max_alloc() const
    {
        return maxAllocNum_;
    }

    int total() const
    {
        zl::thread::LockGuard<LockType> lock(mutex_);
        return totalAllocNum_;
    }

    int avail() const
    {
        zl::thread::LockGuard<LockType> lock(mutex_);
        return pools_.size();
    }

private:
    bool pre_alloc(int allocNum)
    {
        if(maxAllocNum_ > 0)
            allocNum = allocNum < (maxAllocNum_ - totalAllocNum_) ? allocNum : (maxAllocNum_ - totalAllocNum_);
        if(allocNum <= 0)
            return false;

        T* chunk = new T[allocNum];
        chunks_.push_back(chunk);

        for(int i = 0; i < allocNum; i++)
        {
            T *t = &chunk[i];
            pools_.push_back(t);
        }

        totalAllocNum_ += allocNum;
        return true;
    }

private:
    mutable LockType         mutex_;    // guard(single thread or multithread)
    int              maxAllocNum_;      // max alloc num of T, 0 means infinite
    int              totalAllocNum_;    // total alloc num of T
    std::list<T*>    pools_;            // available instance of T
    std::vector<T*>  chunks_;           // save all memory from "new"
};

} } // namespace zl{ namespace base {
#endif  /* ZL_OBJECTPOOL_H */
