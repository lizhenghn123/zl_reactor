// ***********************************************************************
// Filename         : SelectPoller.h
// Author           : LIZHENG
// Created          : 2015-01-13
// Description      : I/O MultiPlexing 的 select 实现
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SELECTPOLLER_H
#define ZL_SELECTPOLLER_H
#include "net/poller/Poller.h"
#include <set>
#ifdef OS_LINUX
#include <sys/select.h>
#endif
NAMESPACE_ZL_NET_START

class SelectPoller : public Poller
{
public:
    explicit SelectPoller(EventLoop *loop);

    ~SelectPoller();

public:
    virtual bool updateChannel(Channel *channel);

    virtual bool removeChannel(Channel *channel);

    virtual Timestamp poll_once(int timeoutMs, ChannelList& activeChannels);

    virtual const char* ioMultiplexerName() const { return "select"; }

private:
    void fireActiveChannels(int numEvents, ChannelList& activeChannels) const;

private:
    fd_set readfds_;           /// select返回的所有可读事件
    fd_set writefds_;          /// select返回的所有可写事件
    fd_set exceptfds_;         /// select返回的所有错误事件

    fd_set select_readfds_;    /// 加入到select中的感兴趣的所有可读事件
    fd_set select_writefds_;   /// 加入到select中的感兴趣的所有可写事件
    fd_set select_exceptfds_;  /// 加入到select中的感兴趣的所有错误事件

    std::set< int, std::greater<int> >  fdlist_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_SELECTPOLLER_H */
