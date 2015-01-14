// ***********************************************************************
// Filename         : Channel.h
// Author           : LIZHENG
// Created          : 2014-10-26
// Description      :
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-01-12
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

// Event types that can be polled for.  These bits may be set in `events'
// to indicate the interesting event types; they will appear in `revents'
// to indicate the status of the file descriptor.  */
#define FDEVENT_NONE    0x000       /* nothing */

// these are the POLL* values from <bits/poll.h> (linux poll)
#define FDEVENT_IN		0x001		/* There is data to read.  */
#define FDEVENT_PRI		0x002		/* There is urgent data to read.  */
#define FDEVENT_OUT		0x004		/* Writing now will not block.  */

// Event types always implicitly polled for.  These bits need not be set in `events',
// but they will appear in `revents' to indicate the status of the file descriptor.
#define FDEVENT_ERR		0x008		/* Error condition.  */
#define FDEVENT_HUP		0x010		/* Hung up.  */
#define FDEVENT_NVAL	0x020		/* Invalid polling request.  */

#define FDEVENT_RDHUP   0x2000      /* gnu extendsion */

enum 
{
	kEventNone    = FDEVENT_NONE,
    kEventRead    = FDEVENT_IN | FDEVENT_PRI,
    kEventWrite   = FDEVENT_OUT,
	kEventError   = FDEVENT_ERR
};

class Channel : zl::NonCopy
{
public:
    typedef std::function<void()>          EventCallback;
    typedef std::function<void(Timestamp)> ReadEventCallback;

public:
    Channel(EventLoop* loop, int fd);
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
    { return events_ == kEventNone; }

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

private:
    EventLoop  *loop_;
    int        fd_;       // fd_ may be socket\signal\timerfd
    int        events_;
    int        revents_;  // events of the poller returned

    ReadEventCallback readCallback_;
    EventCallback     writeCallback_;
    EventCallback     closeCallback_;
    EventCallback     errorCallback_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_CHANNEL_H */
