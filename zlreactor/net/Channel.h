// ***********************************************************************
// Filename         : Channel.h
// Author           : LIZHENG
// Created          : 2014-10-26
// Description      :
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-26
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_CHANNEL_H
#define ZL_CHANNEL_H
#include "Define.h"
#include "net/SocketUtil.h"
#include "base/Timestamp.h"
#include "base/NonCopy.h"
NAMESPACE_ZL_NET_START

class EventLoop;
using zl::base::Timestamp;

//enum EventType
//{
//    kEventNone  = 0,
//    kEventRead  = 1<<0,       //POLLIN | POLLPRI | POLLRDHUP
//    kEventWrite = 1<<1,       //POLLOUT
//    kEventPri   = 1<<2,       //POLLPRI
//    kEventHup   = 1<<3,       //POLLHUP
//    kEventError = 1<<4        //POLLERR | POLLNVAL
//};

class Channel : zl::NonCopy
{
public:
    typedef std::function<void()>          EventCallback;
    typedef std::function<void(Timestamp)> ReadEventCallback;

public:
    Channel(EventLoop* loop, ZL_SOCKET fd);
    ~Channel();

    ZL_SOCKET fd() const { return fd_; }

    EventLoop* ownerLoop() { return loop_; }

    void setReadCallback(const ReadEventCallback& cb)
    { readCallback_ = cb; }

    void setWriteCallback(const EventCallback& cb)
    { writeCallback_ = cb; }

    void setCloseCallback(const EventCallback& cb)
    { closeCallback_ = cb; }

    void setErrorCallback(const EventCallback& cb)
    { errorCallback_ = cb; }

    int events() const
    { return events_; }

    void set_revents(int revt) // used by pollers, set return events
    { revents_ = revt; }
    
    int revents() const
    { return revents_; }

    bool isNoneEvent() const
    { return events_ == Channel::kEventNone; }

    void enableReading()
    { events_ |= kEventRead; update(); }

    void disableReading() 
    { events_ &= ~kEventRead; update(); }

    void enableWriting() 
    { events_ |= kEventWrite; update(); }

    void disableWriting() 
    { events_ &= ~kEventWrite; update(); }

    void disableAll() 
    { events_ = kEventNone; update(); }

    bool isWriting() const
    { return events_ & kEventWrite; }

    void handleEvent(Timestamp receiveTime);
    void remove();
    std::string reventsToString() const;

private:
    void update();
    void handleEventWithHold(Timestamp receiveTime);

    static const int kEventNone;
    static const int kEventRead;
    static const int kEventWrite;

    EventLoop  *loop_;
    ZL_SOCKET  fd_;
    int        events_;
    int        revents_;  // events of the poller returned

    ReadEventCallback readCallback_;
    EventCallback     writeCallback_;
    EventCallback     closeCallback_;
    EventCallback     errorCallback_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_CHANNEL_H */