// ***********************************************************************
// Filename         : Timerfd.h
// Author           : LIZHENG
// Created          : 2014-11-23
// Description      : timerfd need linux kernel > 2.6.25
//                    Linux下基于基于文件描述符的定时器接口，通过fd的可读事件进行超时通知，能够用于select/poll
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_TIMERFD_H
#define ZL_TIMERFD_H
#include "Define.h"
#include "base/Timestamp.h"
#include <sys/timerfd.h>
using zl::base::Timestamp;
NAMESPACE_ZL_NET_START

typedef int Timerfd;

class TimerfdHandler
{
public:
    TimerfdHandler(int clockid = CLOCK_MONOTONIC, int flags = TFD_NONBLOCK | TFD_CLOEXEC);
    ~TimerfdHandler();

public:
    /// 返回定时器描述符
    Timerfd   fd() { return timerfd_; }

    /// 设置新的超时时间(绝对时间)以及定时器循环间隔(<=0 表示只定时一次), 单位：微秒
    void      resetTimerfd(Timestamp expiration, int interval_us = 0);

    /// 设置新的超时时间(相对时间)以及定时器循环间隔(<=0 表示只定时一次), 单位：微秒
    void      resetTimerfd(uint64_t next_expire_us, int interval_us = 0);

    /// 获得当前有多少个定时器超时
    uint64_t  read(uint64_t *howmany);

    /// 停止定时器
    void      stop();

private:
    Timerfd timerfd_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_TIMERFD_H */
