// ***********************************************************************
// Filename         : TriggerTimer.h
// Author           : LIZHENG
// Created          : 2014-07-26
// Description      : 定时器类
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-07-26
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef TRIGGERTIMER_H
#define	TRIGGERTIMER_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <list>

class TriggerTimer
{
    friend class TriggerTimerManager;
public:
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
    TriggerTimer(unsigned int interval,void (*func)(TriggerTimer *,void *),void *args,TimerType type = TIMER_ONCE);
    ~TriggerTimer();
 
    void Start(); // 开始定时器
    void Stop();  // 停止定时器
    void Reset(unsigned int interval); // 重置定时器
    bool Trigger(); // 触发定时器
    
private:   
    TimerState    timer_state_;        //状态
    TimerType     timer_type_;         //类型  
    unsigned int  timer_interval_;     //定时间隔
    unsigned int  timer_duration_;     //开始设置为interval，随延迟时间到，减少
    unsigned long trigger_sum_;        //触发计数
    
    void         *args_;               //回调函数参数
    void (*callback_)(TriggerTimer *,void *); //回调函数
};


class TriggerTimerManager
{
public:    
    static TriggerTimerManager * instance();
    
    void Start();   
    void Stop();  
    void AddTimer(TriggerTimer * vtimer);   
    void DeleteTimer(TriggerTimer * vtimer); 
    
private:    
    void AddTimer_(TriggerTimer * vtimer); 
    void DeleteTimer_(TriggerTimer * vtimer); 
    
private:
    static void* ProcessThread(void *); //定时器延迟线程

    static pthread_mutex_t m_mutex;
    static TriggerTimerManager * m_instance;
    TriggerTimerManager();

    bool running_;
    std::list<TriggerTimer*> timers_;
};

#endif	/* TRIGGERTIMER_H */

