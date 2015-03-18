// ***********************************************************************
// Filename         : HttpServer.h
// Author           : LIZHENG
// Created          : 2015-03-16
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_HTTPSERVER_H
#define ZL_HTTPSERVER_H
#include "Define.h"
#include "net/TcpServer.h"
#include "net/CallBacks.h"
#include "net/http/HttpProtocol.h"
NAMESPACE_ZL_NET_START
class EventLoop;
class HttpRequest;
class HttpResponse;
class InetAddress;

class HttpServer : public TcpServer
{
public:
    HttpServer(EventLoop *loop, const InetAddress& listenAddr, const string& servername = "HttpServer");
    ~HttpServer();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, NetBuffer *buf, Timestamp receiveTime);
    void response(const TcpConnectionPtr& conn, const HttpRequest& req);

private:
    HttpServer(const HttpServer&);
    HttpServer& operator = (const HttpServer&);
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_HTTPSERVER_H */