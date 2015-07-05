// ***********************************************************************
// Filename         : EpollPoller.h
// Author           : LIZHENG
// Created          : 2014-09-26
// Description      : I/O MultiPlexing 的 epoll 实现
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_EPOLLPOLLER_H
#define ZL_EPOLLPOLLER_H
#include "net/poller/Poller.h"
struct epoll_event;
NAMESPACE_ZL_NET_START

class EpollPoller : public Poller
{
public:
    explicit EpollPoller(EventLoop *loop, bool enableET = false);

    ~EpollPoller();

public:
    virtual bool updateChannel(Channel *channel);

    virtual bool removeChannel(Channel *channel);

    virtual Timestamp poll_once(int timeoutMs, ChannelList& activeChannels);

    virtual const char* ioMultiplexerName() const { return "linux_epoll"; }

private:
    bool update(Channel *channel, int operation);

    void fireActiveChannels(int numEvents, ChannelList& activeChannels) const;

private:
    typedef std::vector<struct epoll_event> EpollEventList;

    int  epollfd_;
    bool enableET_;
    EpollEventList events_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_EPOLLPOLLER_H */
