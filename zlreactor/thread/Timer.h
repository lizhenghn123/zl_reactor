// ***********************************************************************
// Filename         : Timer.h
// Author           : LIZHENG
// Created          : 2014-10-126
// Description      : 还有问题，在Windows下select调用不正常
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-26
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_TIMER_H
#define ZL_TIMER_H
#include "Define.h"
#include "base/NonCopy.h"
#include "thread/Mutex.h"
NAMESPACE_ZL_THREAD_START

class Timer
{
    friend class TimerQueue;
public:
     typedef int32_t  TimerID;
     enum TimerState
     {
         TIMER_IDLE=0,  //start前以及手动调用stop后的状态
         TIMER_ALIVE,   //在manager的list里时候的状态
         TIMER_TIMEOUT  //超时后被移除的状态，循环型的没有
     };
     enum TimerType
     {
         TIMER_ONCE=0,  //一次型
         TIMER_CIRCLE   //循环型
     };
public:
    Timer(unsigned int interval,void (*func)(Timer *,void *),void *args,TimerType type = TIMER_ONCE);
    ~Timer();

    void start(); // 开始定时器
    void stop();  // 停止定时器
    void reset(unsigned int interval); // 重置定时器
    bool trigger(); // 触发定时器

private:   
    TimerState    timer_state_;        //状态
    TimerType     timer_type_;         //类型  
    unsigned int  timer_interval_;     //定时间隔
    unsigned int  timer_duration_;     //开始设置为interval，随延迟时间到，减少
    unsigned long trigger_sum_;        //触发计数

    void         *args_;               //回调函数参数
    void (*callback_)(Timer *,void *); //回调函数
public:

};

NAMESPACE_ZL_THREAD_END
#endif  /* ZL_TIMER_H */