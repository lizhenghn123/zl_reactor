// ***********************************************************************
// Filename         : CallBacks.h
// Author           : LIZHENG
// Created          : 2014-10-31
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-31
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_CALLBACKS_H
#define ZL_CALLBACKS_H
NAMESPACE_ZL_NET_START

//using namespace zl::base;
class ByteArray;
class EventLoop;
class TcpConnection;
class InetAddress;
class Acceptor;
class NetBuffer;
using zl::base::Timestamp;

//typedef std::string Buffer;
//typedef ByteArray  Buffer;
//typedef ByteArray*  BufferPtr;
//typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef TcpConnection* TcpConnectionPtr;

void defaultConnectionCallback(TcpConnectionPtr conn);
void defaultMessageCallback(TcpConnectionPtr conn, NetBuffer* buffer, Timestamp receiveTime);

typedef std::function<void()> TimerCallback;
typedef std::function<void (TcpConnection*)> ConnectionCallback;
typedef std::function<void (TcpConnection*)> CloseCallback;
typedef std::function<void (TcpConnection*)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&, NetBuffer*, Timestamp)> MessageCallback;

NAMESPACE_ZL_NET_END
#endif  /* ZL_CALLBACKS_H */