// ***********************************************************************
// Filename         : ActiveSocket.h
// Author           : LIZHENG
// Created          : 2014-07-04
// Description      : 用于主动发起连接请求的socket类，比如client向server的连接
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-07-04
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_ACTIVESOCKET_H
#define ZL_ACTIVESOCKET_H
#include "net/Socket.h"

NAMESPACE_ZL_NET_START

class ActiveSocket : public Socket
{
public:
    explicit ActiveSocket(ZL_SOCKET fd);
    ActiveSocket(const char *host, int port);
    ~ActiveSocket();

public:
    const ActiveSocket& operator << (const std::string&) const;
    const ActiveSocket& operator >> (std::string&) const;
};

NAMESPACE_ZL_NET_END

#endif  /* ZL_ACTIVESOCKET_H */
