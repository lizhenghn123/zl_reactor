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
    EpollPoller(ZL_SOCKET listenfd, int event_size = MAX_EPOLL_EVENTS, bool enableET = false);
    ~EpollPoller();

public:
	virtual bool updateChannel(Channel* channel);

	virtual bool removeChannel(Channel* channel);

    /*
     * 得到可响应读写事件的所有连接
     *
     * @param timeout     : 超时时间(单位:ms)
     * @param activeConns : 已激活的连接
     * @return 激活的连接数，0为超时, -1为异常
     */
	virtual Timestamp poll(int timeoutMs, ChannelList& activeChannels);

private:
    int epollfd_;    // epoll的fd
    bool enableET_;
    ZL_SOCKET listenfd_;

};

NAMESPACE_ZL_NET_END

#endif  /* ZL_EPOLLPOLLER_H */
