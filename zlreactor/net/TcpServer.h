// ***********************************************************************
// Filename         : TcpServer.h
// Author           : LIZHENG
// Created          : 2014-10-31
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-31
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_TCPSERVER_H
#define ZL_TCPSERVER_H
#include "Define.h"
#include "base/Timestamp.h"
#include "thread/Mutex.h"
#include "net/CallBacks.h"
#include "net/InetAddress.h"
#include "base/NonCopy.h"
NAMESPACE_ZL_NET_START
using zl::base::Timestamp;
class ByteBuffer;
class EventLoop;
class Tcpconnection;
class InetAddress;
class Acceptor;

class TcpServer : zl::NonCopy
{
public:
    TcpServer(EventLoop *loop, const InetAddress& listenAddr, const std::string& server_name = "TcpServer");
    ~TcpServer();
    void start();

public:
    EventLoop* getLoop() const
    { return loop_; }

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

private:
    EventLoop *loop_;         // acceptor eventloop
    Acceptor  *acceptor_;
    InetAddress serverAddr_;

    ConnectionCallback    connectionCallback_;
    MessageCallback       messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    typedef std::map<int, TcpConnectionPtr> ConnectionMap;
    ConnectionMap connections_;

    const std::string serverName_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_TCPSERVER_H */