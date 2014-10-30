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

struct epoll_event;

NAMESPACE_ZL_NET_START

class EpollPoller : public Poller
{
public:
    EpollPoller(EventLoop *loop, bool enableET = false);
    ~EpollPoller();

public:
	virtual bool updateChannel(Channel* channel);

	virtual bool removeChannel(Channel* channel);

	virtual Timestamp poll(int timeoutMs, ChannelList& activeChannels);

private:
	typedef std::vector<struct epoll_event> EpollEventList;

    int  epollfd_;     
    bool enableET_;
	EpollEventList events_;
};

NAMESPACE_ZL_NET_END

#endif  /* ZL_EPOLLPOLLER_H */
