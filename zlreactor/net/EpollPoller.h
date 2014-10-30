// ***********************************************************************
// Filename         : EpollPoller.h
// Author           : LIZHENG
// Created          : 2014-09-26
// Description      : I/O MultiPlexing 的 Epoll实现
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-09-26
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_EPOLLPOLLER_H
#define ZL_EPOLLPOLLER_H
#include "net/Poller.h"

NAMESPACE_ZL_NET_START

#define MAX_EPOLL_EVENTS 1024

class EpollPoller : public Poller
{
public:
    EpollPoller(EventLoop *loop, ZL_SOCKET listenfd, int event_size = MAX_EPOLL_EVENTS, bool enableET = false);
    ~EpollPoller();

public:
	virtual bool updateChannel(Channel* channel);

	virtual bool removeChannel(Channel* channel);

	virtual Timestamp poll(int timeoutMs, ChannelList& activeChannels);

private:
    int  epollfd_;    // epoll的fd
    bool enableET_;
    ZL_SOCKET listenfd_;

};

NAMESPACE_ZL_NET_END

#endif  /* ZL_EPOLLPOLLER_H */
