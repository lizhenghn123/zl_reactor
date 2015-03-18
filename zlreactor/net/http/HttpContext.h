// ***********************************************************************
// Filename         : HttpContext.h
// Author           : LIZHENG
// Created          : 2015-03-18
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_HTTPCONTEXT_H
#define ZL_HTTPCONTEXT_H
#include "Define.h"
#include "base/NonCopy.h"
#include "net/TcpServer.h"
#include "net/CallBacks.h"
#include "net/http/HttpProtocol.h"
#include "net/http/HttpRequest.h"
NAMESPACE_ZL_NET_START
class EventLoop;
class HttpRequest;
class HttpResponse;
class InetAddress;
class NetBuffer;

class HttpContext// : public zl::NonCopy
{
public:
    enum HttpRequestParseState
    {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,
    };

    HttpContext()
        : state_(kExpectRequestLine)
    {
    }

    bool expectRequestLine() const { return state_ == kExpectRequestLine; }
    bool expectHeaders() const     { return state_ == kExpectHeaders; }
    bool expectBody() const        { return state_ == kExpectBody; }
    bool gotAll() const            { return state_ == kGotAll; }
    void receiveRequestLine()      { state_ = kExpectHeaders; }
    void receiveHeaders()          { state_ = kGotAll; }  // FIXME

    void reset()
    {
        state_ = kExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }

    HttpRequest& request()     { return request_; }
    const HttpRequest& request() const { return request_; }

public:
    bool parseRequest(NetBuffer *buf, Timestamp receiveTime);
    bool processRequestLine(const char* begin, const char* end);

private:
    HttpRequestParseState state_;
    HttpRequest request_;
};

NAMESPACE_ZL_NET_END
#endif  /* ZL_HTTPCONTEXT_H */
