// ***********************************************************************
// Filename         : TimerQueue.h
// Author           : LIZHENG
// Created          : 2014-10-126
// Description      :
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-26
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_TIMERQUEUE_H
#define ZL_TIMERQUEUE_H
//#include <WinSock2.h>
#include "Define.h"
#include "base/NonCopy.h"
#include "thread/Mutex.h"
#include "thread/Thread.h"
NAMESPACE_ZL_THREAD_START

class Timer;

class TimerQueue
{
public:    
    static TimerQueue * instance();
    TimerQueue();
    ~TimerQueue()
    {

    }
    void start();   
    void stop();  
    void addTimer(Timer * vtimer);   
    void deleteTimer(Timer * vtimer); 

private:    
    void addTimer_(Timer * vtimer); 
    void deleteTimer_(Timer * vtimer); 

private:
    void processThread(); //定时器延迟线程

    static zl::thread::Mutex m_mutex;
    static TimerQueue * m_instance;

    bool running_;
    Thread thread_;
    std::list<Timer*> timers_;
};

NAMESPACE_ZL_THREAD_END
#endif  /* ZL_TIMERQUEUE_H */