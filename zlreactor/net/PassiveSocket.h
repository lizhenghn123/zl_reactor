// ***********************************************************************
// Filename         : PassiveSocket.h
// Author           : LIZHENG
// Created          : 2014-07-04
// Description      : 用于被动接受连接请求的socket类，比如server接收client的连接
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-07-04
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_PASSIVESOCKET_H
#define ZL_PASSIVESOCKET_H
#include "net/Socket.h"

NAMESPACE_ZL_NET_START

class PassiveSocket : public Socket
{
public:
    explicit PassiveSocket(ZL_SOCKET fd);
    PassiveSocket(const char *ip, int port);
    ~PassiveSocket();

public:
    const PassiveSocket& operator << (const std::string&) const;
    const PassiveSocket& operator >> (std::string&) const;
};

NAMESPACE_ZL_NET_END

#endif  /* ZL_PASSIVESOCKET_H */
