// ***********************************************************************
// Filename         : TimerQueue.h
// Author           : LIZHENG
// Created          : 2014-10-126
// Description      : 一个通用的定时器管理
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
#include "base/Timestamp.h"
using zl::base::Timestamp;
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
    void processThread();   //定时器延迟线程

private:
    typedef std::multimap<Timestamp, Timer*>  TimerList;

    bool                running_;
    bool                checkTimer_;

    TimerList           timers_;
    std::vector<Timer*> addTimes_;
    std::vector<Timer*> delTimes_;

    zl::thread::Mutex   mutex_;
    zl::thread::Thread  thread_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_TIMERQUEUE_H */
