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
#include "base/Timestamp.h"
#include "net/http/HttpProtocol.h"
using zl::base::Timestamp;
NAMESPACE_ZL_NET_START

class HttpRequest
{
public:
    HttpRequest();
    HttpRequest(const std::string& header);
    ~HttpRequest();

public:
    void setHeader(const string& header)     { header_ = header; }
    const std::string& getHeader() const     { return header_; }

    void setMethod(HttpMethod method)        { method_ = method; }
    bool setMethod(const string& method)
    {
        if (method == "GET")
            method_ = HttpGet;
        else if (method == "POST")
            method_ = HttpPost;
        else if (method == "HEAD")
            method_ = HttpHead;
        else if (method == "PUT")
            method_ = HttpPut;
        else if (method == "DELETE")
            method_ = HttpDelete;
        else
            method_ = HttpInvalid;
        return method_ != HttpInvalid;
    }
    HttpMethod method() const                { return method_; }

    void setVersion(HttpVersion httpver)     { version_ = httpver; }
    void setVersion(const string& httpver)
    {
        version_ = (httpver == "HTTP/1.1" ? HTTP_VERSION_1_1 : HTTP_VERSION_1_0);
    }
    HttpVersion version() const              { return version_; }

    void setPath(const std::string& url)     { urlpath_ = url; }
    const std::string& path() const          { return urlpath_; }

    void setQuery(const std::string& url)    { urlpath_ = url; }
    const std::string& query() const         { return urlpath_; }

    void setReceiveTime(Timestamp t)         { receiveTime_ = t; }
    Timestamp receiveTime() const            { return receiveTime_; }

    void addHeader(const char* start, const char* colon, const char* end)
    {
        string field(start, colon);
        ++colon;
        while (colon < end && isspace(*colon))
        {
            ++colon;
        }
        string value(colon, end);
        while (!value.empty() && isspace(value[value.size()-1]))
        {
            value.resize(value.size()-1);
        }
        headers_[field] = value;
    }

    string getHeader(const string& field) const
    {
        string result;
        std::map<string, string>::const_iterator it = headers_.find(field);
        if (it != headers_.end())
        {
            result = it->second;
        }
        return result;
    }

    const std::map<string, string>& headers() const  { return headers_; }

    void swap(HttpRequest& that)
    {
        std::swap(method_, that.method_);
        std::swap(version_, that.version_);
        urlpath_.swap(that.urlpath_);
        query_.swap(that.query_);
        receiveTime_.swap(that.receiveTime_);
        headers_.swap(that.headers_);
    }

private:
    bool parseHeader();

private:
    HttpMethod   method_;
    HttpVersion  version_;
    std::string  urlpath_;
    std::string  query_;    // url后面的以?分割的参数

    Timestamp    receiveTime_;
    std::string  header_;

    std::map<string, string> headers_;
};

NAMESPACE_ZL_NET_END
#endif /* ZL_HTTPREQUEST_H */