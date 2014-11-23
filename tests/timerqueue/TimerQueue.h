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
#include "Define.h"
#include "base/NonCopy.h"
#include "thread/Mutex.h"
#include "thread/Thread.h"
NAMESPACE_ZL_NET_START

class Timer;

class TimerQueue
{
public:    
    TimerQueue();
    ~TimerQueue();

    void stop();
    void addTimer(Timer *vtimer);
    void deleteTimer(Timer *vtimer);

private:
    void addTimerWithHold(Timer *vtimer);
    void deleteTimerWithHold(Timer *vtimer);

private:
    void processThread(); //定时器延迟线程

    zl::thread::Mutex m_mutex;

    bool running_;
    bool checkTimer_;
    zl::thread::Thread thread_;
    std::set<Timer*> timers_;
    std::vector<Timer*> addTimes_;
    std::vector<Timer*> delTimes_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_TIMERQUEUE_H */
