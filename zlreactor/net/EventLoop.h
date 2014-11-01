// ***********************************************************************
// Filename         : EventLoop.h
// Author           : LIZHENG
// Created          : 2014-10-26
// Description      : io service, 可管理socket，timer，file等io模式
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-26
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_EVENTLOOP_H
#define ZL_EVENTLOOP_H
#include "Define.h"
#include "base/Timestamp.h"
#include "thread/Mutex.h"
NAMESPACE_ZL_NET_START
class Channel;
class Poller;

class EventLoop
{
public:
    typedef std::function<void()> Functor;
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

public:
    void wakeup();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

public:
    void runInLoop(const Functor& func);
    void queueInLoop(const Functor& func);
    //bool IsInLoopThread() const { return threadId_ == CurrentThread::tid(); }
    bool isInLoopThread() const { return true; }
    bool assertInLoopThread() const  { return true; }

private:
    typedef std::vector<Channel*> ChannelList;
    ChannelList activeChannels_;
    Channel *currentActiveChannel_;

    Poller *poller_;
    bool looping_; /* atomic */
    bool quit_; /* atomic and shared between threads, okay on x86, I guess. */
    bool eventHandling_; /* atomic */
    mutable zl::thread::Mutex mutex_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_EVENTLOOP_H */