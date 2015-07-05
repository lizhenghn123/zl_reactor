// ***********************************************************************
// Filename         : TimerQueue.h
// Author           : LIZHENG
// Created          : 2014-12-22
// Description      :
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_TIMERQUEUE_H
#define ZL_TIMERQUEUE_H
#include "Define.h"
#include "base/NonCopy.h"
#include "thread/Mutex.h"
#include "thread/Atomic.h"
#include "net/Timer.h"
#include "CallBacks.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
using zl::base::Timestamp;
NAMESPACE_ZL_NET_START

class Timer;
class EventLoop;

class TimerQueue
{
    typedef std::pair<Timestamp, Timer*>         Entry;
    typedef std::set< Entry >                    TimerList;
    typedef std::unordered_map<TimerId, Timer*>  TimerMap;
    typedef std::unordered_set<TimerId>          CancelTimerList;

public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

public:
    TimerId addTimer(const TimerCallback& cb, const Timestamp& when, double interval);
    void    cancelTimer(TimerId id);
    Timestamp getNearestExpiration() const;
    void    runTimer(const Timestamp& now);

private:
    void addTimerInLoop(Timer* timer);
    void cancelTimerInLoop(TimerId id);
    void addTimer(Timer* timer);
    std::vector< Entry > getExpiredTimers(const Timestamp& now);

private:
    TimerList                timers_;
    TimerMap                 activeTimers_;
    //TimerMap   cancelTimers_;
    CancelTimerList          cancelTimers_;

    EventLoop                *loop_;
    zl::thread::Atomic<int>  atomic_;
    zl::thread::Atomic<bool> callingTimesFunctor_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_TIMERQUEUE_H */
