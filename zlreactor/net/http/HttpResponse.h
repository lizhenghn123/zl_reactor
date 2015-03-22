// ***********************************************************************
// Filename         : HttpResponse.h
// Author           : LIZHENG
// Created          : 2015-03-16
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 
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
    void setStatusCode(HttpStatusCode code) { statusCode_ = code; }
    void setHttpVersion(const std::string& httpver) { version_ = httpver; }
    void setServerName(const std::string& name) { serverName_ = name; }

    void setCloseConnection(bool on)  { closeConnection_ = on; }
    bool closeConnection() const      { return closeConnection_; }

    bool compile();

    void appendToBuffer(NetBuffer* output) const;

protected:
    void readBoby();

private:
    HttpStatusCode       statusCode_;
    string               version_;
    string               serverName_;
    bool                 closeConnection_;
    string body_;
    map<string, string>  headers_;
};

NAMESPACE_ZL_NET_END

# endif /* ZL_HTTPRESPONSE_H */