// ***********************************************************************
// Filename         : Eventfd.h
// Author           : LIZHENG
// Created          : 2015-01-14
// Description      : eventfd need linux kernel > 2.6.22
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-01-14
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_EVENTFD_H
#define ZL_EVENTFD_H
#include "Define.h"
NAMESPACE_ZL_NET_START

class EventfdHandler
{
public:
    EventfdHandler();
    ~EventfdHandler();

public:
    /// iff success, return eventfd_, else return -1;
    int createEventfd();

    int eventfd() { return eventfd_; }

    ssize_t write(const void *data, size_t len);

    ssize_t read(void *buf, size_t size);

private:
    int   eventfd_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_EVENTFD_H */