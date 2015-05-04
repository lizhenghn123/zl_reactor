// ***********************************************************************
// Filename         : Eventfd.h
// Author           : LIZHENG
// Created          : 2015-01-14
// Description      : 进程及线程间的事件通知，need linux kernel > 2.6.22
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-01-14
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_EVENTFD_H
#define ZL_EVENTFD_H
#include "Define.h"
#include <stdint.h> 
#include <sys/eventfd.h>
NAMESPACE_ZL_NET_START

// eventfd的缓冲区大小是sizeof(uint64_t), 也即是8字节.
// 它是一个64位计数器, 计数器不为零是有可读事件发生.
// 写入一次增加计数器值，读取将获取计数器值，并清零.

class EventfdHandler
{
public:
    EventfdHandler(unsigned int initval = 0, int flags = EFD_NONBLOCK | EFD_CLOEXEC);
    ~EventfdHandler();

public:
    int fd() { return eventfd_; }

    ssize_t write(uint64_t value = 1);

    ssize_t read(uint64_t *value = NULL);

private:
    /// iff success, return eventfd_, else return -1;
    int createEventfd(unsigned int initval, int flags);

private:
    int   eventfd_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_EVENTFD_H */
