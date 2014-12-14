// ***********************************************************************
// Filename         : TcpClient.h
// Author           : LIZHENG
// Created          : 2014-11-06
// Description      :
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-11-06
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_TCPCLIENT_H
#define ZL_TCPCLIENT_H
#include "Define.h"
#include "base/Timestamp.h"
#include "thread/Mutex.h"
#include "net/CallBacks.h"
#include "net/InetAddress.h"
#include "base/NonCopy.h"
using zl::base::Timestamp;
using zl::thread::Mutex;

NAMESPACE_ZL_NET_START

class EventLoop;
class InetAddress;
class Connector;
class ByteBuffer;
class Tcpconnection;

class TcpClient
{
public:
    TcpClient(EventLoop* loop, const InetAddress& serverAddr);
    ~TcpClient();

public:
    EventLoop* getLoop() const { return loop_; }

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

public:
    void connect();
    void disconnect();
    void stop();

    bool retry() const;
    void enableRetry() { retry_ = true; }

private:
    void newConnection(int sockfd);
    void removeConnection(const TcpConnectionPtr& conn);

private:
    EventLoop              *loop_;
    Connector              *connector_;
    ConnectionCallback     connectionCallback_;
    MessageCallback        messageCallback_;
    WriteCompleteCallback  writeCompleteCallback_;

    bool                   retry_;
    bool                   connect_;

    TcpConnectionPtr       connection_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_TCPCLIENT_H */
