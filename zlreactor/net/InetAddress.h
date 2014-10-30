// ***********************************************************************
// Filename         : InetAddress.h
// Author           : LIZHENG
// Created          : 2014-09-06
// Description      : InetAddress的跨平台包装
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-09-06
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_INETADDRESS_H
#define ZL_INETADDRESS_H
#include "Define.h"
#include "base/NonCopy.h"
#include "net/SocketUtil.h"
#ifdef OS_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
NAMESPACE_ZL_NET_START

class InetAddress : public zl::NonCopy
{
public:
    explicit InetAddress(uint16_t port = 0);
    InetAddress(const char *ip, uint16_t port);
    InetAddress(const ZL_SOCKADDR_IN& addr);

    static bool resolve(const char *hostname, InetAddress *addr);

public:
    uint16_t port() const;
    std::string ip() const;
    std::string ipPort() const;

    size_t addressLength() const { return sizeof(addr_); }
    operator struct sockaddr *() const{ return (struct sockaddr*)&addr_; }

    const ZL_SOCKADDR_IN& getSockAddrInet() const { return addr_; }
    void setSockAddrInet(const ZL_SOCKADDR_IN& addr) { addr_ = addr; }

    uint32_t ipNetEndian() const { return addr_.sin_addr.s_addr; }
    uint16_t portNetEndian() const { return addr_.sin_port; }

private:
    ZL_SOCKADDR_IN  addr_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_INETADDRESS_H */