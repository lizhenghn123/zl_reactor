// ***********************************************************************
// Filename         : TcpConnector.h
// Author           : LIZHENG
// Created          : 2014-10-26
// Description      : 客户端连接器，连接远程Socket
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-12-22
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_TCPCONNECTOR_H
#define ZL_TCPCONNECTOR_H
#include "Define.h"
#include "base/NonCopy.h"
#include "net/SocketUtil.h"
#include "net/InetAddress.h"
NAMESPACE_ZL_NET_START
class Socket;
class Channel;
class EventLoop;
class InetAddress;

class TcpConnector : zl::NonCopy
{
public:
    typedef std::function<void (ZL_SOCKET)> NewConnectionCallback;

public:
    TcpConnector(EventLoop *loop, const InetAddress& serverAddr);
    ~TcpConnector();

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
    Channel                    *TcpConnector_channel_;
    NewConnectionCallback      newConnCallBack_;
};

typedef TcpConnector* TcpConnectorPtr;

NAMESPACE_ZL_NET_END
#endif  /* ZL_TCPCONNECTOR_H */
