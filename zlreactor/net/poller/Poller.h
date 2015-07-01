// ***********************************************************************
// Filename         : Poller.h
// Author           : LIZHENG
// Created          : 2014-09-26
// Description      : I/O MultiPlexing 抽象接口
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_POLLER_H
#define ZL_POLLER_H
#include <vector>
#include "Define.h"
#include "net/SocketUtil.h"
#include "base/Timestamp.h"
NAMESPACE_ZL_NET_START

#define POLL_WAIT_INDEFINITE

#ifdef OS_WINDOWS
#define USE_POLLER_SELECT
#else
#define USE_POLLER_EPOLL
#define USE_POLLER_SELECT
#define USE_POLLER_POLL
#endif

using zl::base::Timestamp;
class Socket;
class Channel;
class EventLoop;

class Poller
{
public:
    typedef std::vector<Channel *>          ChannelList;
    typedef std::map<ZL_SOCKET, Channel *>  ChannelMap;

public:
    explicit Poller(EventLoop *loop);
    virtual ~Poller();

    /// 根据各种宏定义及操作系统区分创建可用的backends
    /// @param loop        : EventLoop, I/O service
    /// @return            : I/O backends
    static Poller *createPoller(EventLoop *loop);

public:
    /// 添加/更新Channel所绑定socket的I/O events, 必须在主循环中调用 
    /// @param channel     : 待更新的Channel
    /// @return            : 成功为true，失败为false
    virtual bool updateChannel(Channel *channel) = 0;

    /// 删除Channel所绑定socket的I/O events, 必须在主循环中调用
    /// @param channel     : 待删除的Channel
    /// @return            : 成功为true，失败为false
    virtual bool removeChannel(Channel *channel) = 0;

    /// 得到可响应读写事件的所有连接, 必须在主循环中调用
    /// @param timeout     : 超时时间(单位:ms)
    /// @param activeConns : 已激活的连接
    /// @return            : io multiplexing 调用返回时的当前时间
    virtual Timestamp poll_once(int timeoutMs, ChannelList &activeChannels) = 0;

    /// 获得当前所使用的IO复用backends的描述
    /// @return            : IO复用的名称
    virtual const char* ioMultiplexerName() const = 0;

public:
    /// 判断该Channel是否在Poller中
    /// @param channel     : 待删除的Channel
    /// @return            : 存在为true，否则为false
    bool hasChannel(const Channel *channel) const;

    /// 获取当前存在的连接
    /// @param sock        : socket/timer/signal fd
    /// @return            : socket对应的连接, 如不存在返回NULL
    Channel* getChannel(ZL_SOCKET sock) const;

protected:
    ChannelMap  channelMap_;
    EventLoop   *loop_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_POLLER_H */
