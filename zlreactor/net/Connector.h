// ***********************************************************************
// Filename         : Connector.h
// Author           : LIZHENG
// Created          : 2014-10-26
// Description      : 客户端连接器，连接远程Socket
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-26
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_CONNECTOR_H
#define ZL_CONNECTOR_H
#include "Define.h"
#include "base/NonCopy.h"
#include "net/SocketUtil.h"
#include "net/InetAddress.h"
NAMESPACE_ZL_NET_START
class Socket;
class Channel;
class EventLoop;
class InetAddress;

class Connector : zl::NonCopy
{
public:
    typedef std::function<void (ZL_SOCKET)> NewConnectionCallback;

public:
    Connector(EventLoop *loop, const InetAddress& serverAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& callback)
    {
        newConnCallBack_ = callback;
    }

    const InetAddress& serverAddress() const 
    { 
        return serverAddr_; 
    }

    void connect();
    void stop();

private:
    void connectInLoop();
    void connectServer();
    void connectEstablished(ZL_SOCKET sock);
    void stopInLoop();

    void handleWrite();
    void handleError();
    ZL_SOCKET disableChannel();
    void retry(ZL_SOCKET sockfd);

    enum States { kDisconnected, kConnecting, kConnected };
    void setState(States s) { state_ = s; }

private:
    States                     state_; 
    bool                       connect_;
    EventLoop                  *loop_;
    const InetAddress&         serverAddr_;
    Channel                    *connector_channel_;
    NewConnectionCallback      newConnCallBack_;
};

typedef Connector* ConnectorPtr;

NAMESPACE_ZL_NET_END
#endif  /* ZL_CONNECTOR_H */