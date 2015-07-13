// ***********************************************************************
// Filename         : TcpServer.h
// Author           : LIZHENG
// Created          : 2014-10-31
// Description      : 
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
class EventLoopThreadPool;

class TcpServer : zl::NonCopy
{
public:
    TcpServer(EventLoop *loop, const InetAddress& listenAddr, const std::string& server_name = "TcpServer");
    virtual ~TcpServer();

    /// 设置EventLoopThreadPool的threads大小；if numThreads
    /// < 0  : 设置该值为当前系统CPU并发数；
    /// == 0 : 不使用EventLoopThreadPool，所有Channel都在同一个EventLoop中运行，默认值；
    /// > 0  : 设置numThreads个线程，也即numThreads个EventLoop，每个连接选择其中一个
    /// 注意： 该函数必须在start之前调用
    void setMultiReactorThreads(int numThreads);

    /// 启动TcpServer，设置server socket listen
    /// 注意：必须调用该接口，且只限调用一次
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

protected:
    typedef std::map<int, TcpConnectionPtr>     ConnectionMap;
    typedef std::vector<EventLoopThreadPool*>   EventLoopList;
    EventLoop             *loop_;         // acceptor eventloop
    TcpAcceptor           *acceptor_;
    InetAddress           serverAddr_;

    ConnectionCallback    connectionCallback_;
    MessageCallback       messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    ConnectionMap         connections_;
    EventLoopThreadPool   *evloopThreadPool_;

    const std::string     serverName_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_TCPSERVER_H */
