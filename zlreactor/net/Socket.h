// ***********************************************************************
// Filename         : Socket.h
// Author           : LIZHENG
// Created          : 2014-07-01
// Description      : socket的跨平台包装
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-07-01
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SOCKET_H
#define ZL_SOCKET_H
#include <stdio.h>
#include <exception>
#include "OsDefine.h"
#include "SocketUtil.h"
NAMESPACE_ZL_NET_START

class SocketAddress;
class InetAddress;

class SocketException : public std::exception
{
public:
    SocketException(const char *err_msg)
    {
        printf("Get Socket Exception: %s\n", err_msg);
    }
};

class Socket
{
public:
    Socket();
    virtual ~Socket();
public:
    // Server Initialization
    bool           Create();
    bool           Bind(const char *ip, int port);
    bool           Listen(int backlog = 5) const;
    bool           Accept(Socket&) const;
    void           Close();

    // Client Initialization
    bool           Connect(const std::string& host, const int port);

    // Socket Settings
    /** Enable/disable Block Socket */
    bool           SetBlocking();
    bool           SetNonBlocking();

    /** Enable/disable TCP_NODELAY(enable/disable Nagle's algorithm) */
    bool           SetNoDelay(bool flag = true);

    /** Enable/disable SO_REUSEADDR(TIME_WAIT) */
    bool           SetReuseAddr(bool flag = true);

    /** Enable/disable SO_KEEPALIVE */
    bool           SetKeepAlive(bool flag = true);

    /** Set/Get SO_SNDBUF */
    bool           SetSocketReadSize(int readSize);
    bool           GetSocketReadSize(int& readSize);

    /** Set/Get  SO_RCVBUF */
    bool           SetSocketWriteSize(int writeSize);
    bool           GetSocketWriteSize(int& writeSize);

    /** Set/Get  SO_SNDTIMEO */
    bool           SetSendTimeout(int sendTimeoutSec, int sendTimeoutUsec = 0);
    bool           GetSendTimeout(int& sendTimeoutSec, int& sendTimeoutUsec);

    /** Set/Get  SO_RCVTIMEO */
    bool           SetReceiveTimeout(int recvTimeoutSec, int recvTimeoutUsec = 0);
    bool           GetReceiveTimeout(int& recvTimeoutSec, int& recvTimeoutUsec);

    /** Set/Get  SO_LINGER */
    bool           SetLinger(bool enable, int waitTimeSec = 5);
    bool           GetLinger(bool& enable, int& waitTimeSec);

    bool           SetOpt(int level, int name, char *value, int len);
    bool           GetOpt(int level, int optname, int& optval);

    // Net Transimission
    int            Send(const std::string& data) const;
    int            Send(const char *data, size_t size)const;
    int            Recv(std::string& data) const;
    int            Recv(char *data, int length, bool complete = false)const;
    int            SendTo(const std::string& data, int flags, InetAddress& sinaddr)const;
    int            SendTo(const char *data, size_t size, int flags, InetAddress& sinaddr)const;
    int            RecvFrom(std::string& data, int flags, InetAddress& sinaddr)const;
    int            RecvFrom(char *data, int length, int flags, InetAddress& sinaddr)const;

    // Property Access
    bool           IsValid() const
    {
        return sockfd_ != ZL_INVALID_SOCKET;
    }
    const          ZL_SOCKET& GetSocket()const
    {
        return sockfd_;
    }

    short          GetHostPort();
    std::string    GetHostIP();
    std::string    GetHost();    // IP:Port

public://protected:
    ZL_SOCKET       sockfd_;
    ZL_SOCKADDR_IN  sockaddr_;
};


NAMESPACE_ZL_NET_END

#endif  /* ZL_SOCKET_H */
