// ***********************************************************************
// Filename         : TcpConnection.h
// Author           : LIZHENG
// Created          : 2014-10-31
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-31
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_TCPCONNECTION_H
#define ZL_TCPCONNECTION_H
#include "Define.h"
#include "base/Timestamp.h"
#include "net/CallBacks.h"
#include "net/InetAddress.h"
#include "net/Socket.h"
NAMESPACE_ZL_NET_START
class Channel;
class EventLoop;
class Socket;
class InetAddress;

class TcpConnection
{
public:
    TcpConnection(EventLoop* loop, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    ZL_SOCKET fd() { return socket_->getSocket(); }
    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const { return peerAddr_; }

    bool connected() const { return state_ == kConnected; }

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

    void send(const void* message, int len);
    void send(Buffer* message);
    void shutdown();

    Buffer* inputBuffer()
    { return &inputBuffer_; }

    Buffer* outputBuffer()
    { return &outputBuffer_; }

    void setCloseCallback(const CloseCallback& cb)
    { closeCallback_ = cb; }

    // called when TcpServer accepts a new connection
    void connectEstablished();   // should be called only once
    // called when TcpServer has removed me from its map
    void connectDestroyed();  // should be called only once

private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    void sendInLoop(const void* message, size_t len);
    void shutdownInLoop();
    void setState(StateE s) { state_ = s; }

private:
    EventLoop* loop_;
    StateE state_;  

    Socket *socket_;
    Channel *channel_;
    const InetAddress localAddr_;
    const InetAddress peerAddr_;
    ConnectionCallback    connectionCallback_;
    MessageCallback       messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    CloseCallback         closeCallback_;
    size_t highWaterMark_;
    Buffer inputBuffer_;
    Buffer outputBuffer_; // FIXME: use list<Buffer> as output buffer.
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_TCPCONNECTION_H */