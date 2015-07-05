// ***********************************************************************
// Filename         : HttpServer.h
// Author           : LIZHENG
// Created          : 2015-03-16
// Description      : 
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
    typedef std::function<void (const HttpRequest&, HttpResponse*)> HttpCallback;
public:
    HttpServer(EventLoop *loop, const InetAddress& listenAddr, const string& servername = "HttpServer");
    ~HttpServer();

public:
    void setRootDir(const string& dir)     { docRootDir_ = dir; }
    string rootDir()                       { return docRootDir_; }

    void setDefaultPage(const string& page){ defaultPage_ = page; }
    string defaultPage()                   { return defaultPage_; }

    void setCallback(HttpMethod m, const HttpCallback& cb)
    {
        methodCallback_[m] = cb;
    }

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, NetBuffer *buf, Timestamp receiveTime);
    void response(const TcpConnectionPtr& conn, const HttpRequest& req);
    void methodCallback(const HttpRequest& req, HttpResponse *resp);

private:
    HttpServer(const HttpServer&);
    HttpServer& operator = (const HttpServer&);

private:
    std::string    docRootDir_;
    std::string    defaultPage_;
    std::map<HttpMethod, HttpCallback>  methodCallback_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_HTTPSERVER_H */
