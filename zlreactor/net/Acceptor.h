// ***********************************************************************
// Filename         : Acceptor.h
// Author           : LIZHENG
// Created          : 2014-10-26
// Description      :
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-26
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_ACCEPTOR_H
#define ZL_ACCEPTOR_H
#include "Define.h"
#include "base/Timestamp.h"
#include "base/NonCopy.h"
NAMESPACE_ZL_NET_START
class Socket;
class InetAddress;
class Channel;
class EventLoop;
using zl::base::Timestamp;

class Acceptor : zl::NonCopy
{
public:
    //typedef std::function<void(Socket *)> NewConnectionCallback;
    typedef std::function<void (int , const InetAddress&)> NewConnectionCallback;
public:
    Acceptor(EventLoop *loop, const InetAddress& listenAddr);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& callback)
    {
        newConnCallBack_ = callback;
    }

    void listen();

private:
    void onAccept(Timestamp now);
    //    void handleRead();

private:
    EventLoop *loop_;
    Socket    *accept_socket;
    Channel   *accept_channel_;
    NewConnectionCallback newConnCallBack_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_ACCEPTOR_H */