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
NAMESPACE_ZL_NET_START

class EventLoop;
using zl::base::Timestamp;

class Channel
{
public:
    typedef std::function<void()>          EventCallback;
    typedef std::function<void(Timestamp)> ReadEventCallback;
public:
    Channel(EventLoop* loop, ZL_SOCKET fd);
    ~Channel();

    void setReadCallback(const ReadEventCallback& cb)
    {
        readCallback_ = cb;
    }

    void setWriteCallback(const EventCallback& cb)
    {
        writeCallback_ = cb;
    }

    void setCloseCallback(const EventCallback& cb)
    {
        closeCallback_ = cb;
    }

    void setErrorCallback(const EventCallback& cb)
    {
        errorCallback_ = cb;
    }

    ZL_SOCKET fd() const
    {
        return fd_;
    }

    int events() const 
    { 
        return events_; 
    }

    void set_revents(int revt) // used by pollers
    { 
        revents_ = revt; 
    } 
    
    int revents() const 
    { 
        return revents_; 
    }

    bool isNoneEvent() const 
    { 
        return events_ == Channel::kNoneEvent; 
    }

    void enableReading()
    { 
        events_ |= kReadEvent; update(); 
    }

    // void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() 
    { 
        events_ |= kWriteEvent; update();
    }

    void disableWriting() 
    { 
        events_ &= ~kWriteEvent; update(); 
    }

    void disableAll() 
    { 
        events_ = kNoneEvent; update(); 
    }

    bool isWriting() const
    { 
        return events_ & kWriteEvent;
    }

    EventLoop* ownerLoop() { return loop_; }

    std::string reventsToString() const; // for debug

    void remove();

    void handleEvent(Timestamp receiveTime);

public: //private:
    void update();
    void handleEventWithHold(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop  *loop_;
    ZL_SOCKET  fd_;
    int        events_;
    int        revents_;  // the poll return events

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_CHANNEL_H */