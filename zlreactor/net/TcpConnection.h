// ***********************************************************************
// Filename         : TcpConnection.h
// Author           : LIZHENG
// Created          : 2014-10-31
// Description      : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_TCPCONNECTION_H
#define ZL_TCPCONNECTION_H
#include "Define.h"
#include "base/Timestamp.h"
#include "base/NonCopy.h"
#include "stl/any.h"
#include "net/CallBacks.h"
#include "net/InetAddress.h"
#include "net/Socket.h"
#include "net/ByteBuffer.h"
#include "net/Channel.h"
#include <memory>     //for enable_shared_from_this
NAMESPACE_ZL_NET_START
class Channel;
class EventLoop;
class Socket;
class InetAddress;
using zl::base::Timestamp;

class TcpConnection : zl::NonCopy, public std::enable_shared_from_this< TcpConnection > 
{
public:
    TcpConnection(EventLoop* loop, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr);
    ~TcpConnection();

public:
    EventLoop* getLoop() const              { return loop_; }
    ZL_SOCKET fd() const                    { return socket_->fd(); }
    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const  { return peerAddr_; }
    bool connected() const                  { return state_ == kConnected; }

    void setConnectionCallback(const ConnectionCallback& cb)       { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb)             { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }
    void setCloseCallback(const CloseCallback& cb)                 { closeCallback_ = cb; }

    void enableReading()      { channel_->enableReading(); }
    void disableReading()     { channel_->disableReading(); }
    void enableWriting()      { channel_->enableWriting(); }
    void disableWriting()     { channel_->disableWriting(); }
    void disableAll()         { channel_->disableAll(); }
    
    void setNoDelay(bool on)  { socket_->setNoDelay(on); }

    void setContext(const zl::stl::any& context)  { context_ = context; }
	const zl::stl::any getContext() const         { return context_; }
	zl::stl::any* getMutableContext()             { return &context_; }

    void connectEstablished();   // called when TcpServer accepts a new connection
    void connectDestroyed();     // called when TcpServer has removed me from its map

    void send(const void* data, size_t len);
    void send(const std::string& buffer);
    void send(ByteBuffer* buffer);

    void shutdown();

private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    const char* getState(StateE);
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    void sendInLoop(const void* data, size_t len);
    void sendInLoop(const std::string& buffer);
    void shutdownInLoop();
    void setState(StateE s) { state_ = s; }

private:
    EventLoop             *loop_;
    StateE                state_;
    Socket                *socket_;
    Channel               *channel_;
    const InetAddress     localAddr_;
    const InetAddress     peerAddr_;

    zl::stl::any          context_;

    ByteBuffer            inputBuffer_;
    ByteBuffer            outputBuffer_; // FIXME: use list<Buffer> as output buffer.

    ConnectionCallback    connectionCallback_;
    MessageCallback       messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    CloseCallback         closeCallback_;

};

NAMESPACE_ZL_NET_END
#endif  /* ZL_TCPCONNECTION_H */
