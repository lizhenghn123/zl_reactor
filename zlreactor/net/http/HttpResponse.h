// ***********************************************************************
// Filename         : HttpResponse.h
// Author           : LIZHENG
// Created          : 2015-03-16
// Description      : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_HTTPRESPONSE_H
#define ZL_HTTPRESPONSE_H
#include "Define.h"
#include "net/http/HttpProtocol.h"
NAMESPACE_ZL_NET_START

class NetBuffer;
class HttpRequest;

class HttpResponse
{
public:
    explicit HttpResponse(bool closeConn = true);
    ~HttpResponse();

public:
    void setStatusCode(HttpStatusCode code)   { statusCode_ = code; }
    void setVersion(HttpVersion ver)          { version_ = ver; }
    void setServerName(const string& name)    { serverName_ = name; }
    void setContentType(const string& type)   { contentType_ = type; }
    void setCloseConnection(bool on)          { closeConnection_ = on; }
    bool closeConnection() const              { return closeConnection_; }
    void setBody(const string& body)          { body_ = body; }
    void addHeader(const string& key, const string& value)  { headers_[key] = value; }

    void compileToBuffer(NetBuffer* output) const;

private:
    HttpStatusCode       statusCode_;
    HttpVersion          version_;
    string               serverName_;
    string               contentType_;
    bool                 closeConnection_;
    string               body_;
    map<string, string>  headers_;
};

NAMESPACE_ZL_NET_END

# endif /* ZL_HTTPRESPONSE_H */