// ***********************************************************************
// Filename         : Timerfd.h
// Author           : LIZHENG
// Created          : 2014-11-23
// Description      : Linux下基于基于文件描述符的定时器接口，通过fd的可读事件进行超时通知，能够用于select/poll。
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-11-23
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

//创建一个定时器描述符
int createTimerfd(int clockid = CLOCK_MONOTONIC, int flags = TFD_NONBLOCK | TFD_CLOEXEC);


//设置新的超时时间
void resetTimerfd(int timerfd, Timestamp expiration);

//从时间文件描述符获得当前有多少个定时器超时并返回
uint64_t readTimerfd(int timerfd, Timestamp now);

NAMESPACE_ZL_NET_END
#endif  /* ZL_TIMERFD_H */