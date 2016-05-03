// ***********************************************************************
// Filename         : CyclicBarrier.h
// Author           : LIZHENG
// Description      : 回环栅栏，以实现让一组线程等待至某个状态之后再全部同时执行
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_CYCLICbARRIER_H
#define ZL_CYCLICbARRIER_H
#include <functional>
#include "base/NonCopy.h"
#include "thread/Mutex.h"
#include "thread/Condition.h"
NAMESPACE_ZL_THREAD_START

class CyclicBarrier : zl::NonCopy
{
public:
    typedef std::function<void()>     CyclicBarrierCallBack; 
    
public:
    explicit CyclicBarrier(int parties, CyclicBarrier::CyclicBarrierCallBack cb = NULL)
        : parties_(parties)
        , count_(parties)
        , mutex_()
        , condition_(mutex_)
        , cb_(cb)
    {
    }

    void reset()
    {
        LockGuard<Mutex> lock(mutex_);
        nextGeneration();
    }
    
    int wait()
    {
        return dowait(false, 0);
    }
    
    /// Returns the number of parties required to trip this barrier.
    int getParties() const
    {
        return parties_;
    }
    
    /// Returns the number of parties currently waiting at the barrier.
    /// This method is primarily useful for debugging and assertions.
    int getNumberWaiting()
    {
        LockGuard<Mutex> lock(mutex_);
        return parties_ - count_;
    }
    
private:  
    void nextGeneration()
    {
        // signal completion of last generation
        condition_.notify_all();
        // set up next generation
        count_ = parties_;
    }
    
    int dowait(bool timed, int millisecond)
    {
        LockGuard<Mutex> lock(mutex_);
        int index = --count_;
        if (index == 0) // tripped
        {
            if(cb_)
            {
                cb_();
            }
            nextGeneration();
            return 0;         
        }
        
        // loop until tripped, broken, interrupted, or timed out
        for (;;)
        {
            if (!timed)
                condition_.wait();
            else if (millisecond > 0)
                condition_.timed_wait(millisecond);
                
            return index;
        }
    } 
private:
    int              parties_;  /// The number of parties
    int              count_;    /// Number of parties still waiting. Counts down from parties to 0
    mutable Mutex    mutex_;
    Condition        condition_;
    CyclicBarrierCallBack  cb_;
};

NAMESPACE_ZL_THREAD_END

#endif /* ZL_CYCLICbARRIER_H */
