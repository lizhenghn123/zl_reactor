// ***********************************************************************
// Filename         : Poller.h
// Author           : LIZHENG
// Created          : 2014-09-26
// Description      : I/O MultiPlexing 抽象接口
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-09-26
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

using zl::base::Timestamp;
class Socket;
class Channel;
class EventLoop;

class Poller
{
public:
    typedef std::vector<Channel *>          ChannelList;
    typedef std::map<ZL_SOCKET, Channel *, std::greater<ZL_SOCKET> >  ChannelMap;

public:
    Poller(EventLoop *loop);

    virtual ~Poller();

public:
	/// Changes the interested I/O events. Must be called in the loop thread.
	virtual bool updateChannel(Channel* channel) = 0;

    /// Changes the interested I/O events. Must be called in the loop thread.
	virtual bool removeChannel(Channel* channel) = 0;

    /*
     * 得到可响应读写事件的所有连接, 必须在主线程中循环调用
     *
     * @param timeout     : 超时时间(单位:ms)
     * @param activeConns : 已激活的连接
     * @return 当前时间
     */
	virtual Timestamp poll(int timeoutMs, ChannelList& activeChannels) = 0;

	virtual bool hasChannel(const Channel* channel) const;

public:
    /*
     * 获取当前存在的连接
     * @param sock     : 连接socket
     * @return socket对应的连接, 如不存在返回NULL
     */
	Channel* getChannel(ZL_SOCKET sock);

protected:
    ChannelMap  channelMap_;
    EventLoop   *loop_;
};

NAMESPACE_ZL_NET_END

#endif  /* ZL_POLLER_H */
