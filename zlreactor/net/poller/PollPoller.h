// ***********************************************************************
// Filename         : PollPoller.h
// Author           : LIZHENG
// Created          : 2014-12-22
// Description      : I/O MultiPlexing 的 poll 实现
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_POLLPOLLER_H
#define ZL_POLLPOLLER_H
#include "zlreactor/net/poller/Poller.h"
#include <unordered_map>
//#ifdef __GNUC__
//#include <ext/hash_map>
//#else  
//#include <hash_map>
//#endif 
//namespace std
//{
//    using namespace __gnu_cxx;
//}
NAMESPACE_ZL_NET_START

class PollPoller : public Poller
{
public:
    explicit PollPoller(EventLoop *loop);

    ~PollPoller();

public:
    virtual bool updateChannel(Channel *channel);

    virtual bool removeChannel(Channel *channel);

    virtual Timestamp pollOnce(int timeoutMs, ChannelList& activeChannels);

    virtual const char* ioMultiplexerName() const { return "linux_poll"; }

private:
    void fireActiveChannels(int numEvents, ChannelList& activeChannels) const;

private:
    typedef std::vector<struct pollfd>         PollFdList;
    typedef std::unordered_map<Channel*, int>  ChannelIter;

    PollFdList    pollfds_;
    ChannelIter   channelIter_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_POLLPOLLER_H */
