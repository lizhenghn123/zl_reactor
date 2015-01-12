// ***********************************************************************
// Filename         : Timer.h
// Author           : LIZHENG
// Created          : 2014-12-22
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-12-22
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_TIMER_H
#define ZL_TIMER_H
#include "Define.h"
#include "base/NonCopy.h"
#include "thread/Mutex.h"
#include "CallBacks.h"
NAMESPACE_ZL_NET_START
class EventLoop;


class Timer
{
public:
    Timer(int id, const TimerCallback& cb, const Timestamp& when, double interval)
        : id_(id), callback_(cb), when_(when), interval_(interval)
    {

    }

    int  id() const { return id_; }

    Timestamp expires_at() const { return when_; }

    bool repeat() const { return interval_ > 0; }

    void trigger() const { callback_(); }

    void restart(const Timestamp& now)
    {
        if(repeat())
        {
            when_ = now + interval_;
        }
        else
        {
            when_ = Timestamp::invalid();
        }
    }

private:
    int           id_;
    TimerCallback callback_;
    Timestamp     when_;
    double        interval_;  // second
};

inline bool operator<(const Timer& lhs, const Timer& rhs)
{
    return lhs.expires_at() < rhs.expires_at();
}

NAMESPACE_ZL_NET_END
#endif  /* ZL_TIMER_H */