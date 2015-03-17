// ***********************************************************************
// Filename         : HttpRequest.h
// Author           : LIZHENG
// Created          : 2015-03-16
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_HTTPREQUEST_H
#define ZL_HTTPREQUEST_H
#include "Define.h"
#include "net/http/HttpProtocol.h"
NAMESPACE_ZL_NET_START

class HttpRequest
{
public:
    HttpRequest();
    HttpRequest(const std::string& header);
    HttpRequest(const std::string& header, const std::string& document);
    ~HttpRequest();

public:
    void setHeader(const string& header)     { header_ = header; }
    const std::string& getHeader() const     {  return header_; }

    void setDocument(const string& document) { document_ = document; }
    const std::string& getDocument() const   { return document_; }

    void setHttpMethod(HttpMethod method)    { method_ = method; }
    HttpMethod getHttpMethod()               { return method_; }

    void setHttpVersion(HttpVersion httpver) { version_ = httpver; }
    HttpVersion getHttpVersion() const       { return version_; }

    void setHttpUrl(const std::string& url)  { urlpath_ = url; }
    const std::string& getHttpUrl() const    { return urlpath_; }

private:
    bool parseHeader();
    bool parseDocument();

private:
    HttpMethod   method_;
    HttpVersion  version_;
    std::string  urlpath_;

    std::string header_;
    std::string document_;

    std::map<string, string> headers_;
};

NAMESPACE_ZL_NET_END
#endif /* ZL_HTTPREQUEST_H */