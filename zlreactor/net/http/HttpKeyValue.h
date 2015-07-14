// ***********************************************************************
// Filename         : HttpKeyValue.h
// Author           : LIZHENG
// Created          : 2014-07-02
// Description      : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_HTTPKEYVALUE_H
#define ZL_HTTPKEYVALUE_H
#include "Define.h"
#include "base/Singleton.h"
#include "HttpProtocol.h"
NAMESPACE_ZL_NET_START

class HttpKeyValue : public zl::Singleton < HttpKeyValue >
{
    DECLARE_SINGLETON_CLASS(HttpKeyValue);

public:
    std::string getStatusDesc(HttpStatusCode code) const;
    std::string getContentType(const std::string& file_type) const;
    std::string getMethodStr(HttpMethod method) const;

private:
    std::map<HttpStatusCode, std::string>  code_desc_;
    std::map<std::string, std::string>     content_type_;
    std::map<HttpMethod, std::string>      method_str_;

private:
    HttpKeyValue();
    void initialise();
};

NAMESPACE_ZL_NET_END
#endif /* ZL_HTTPKEYVALUE_H */
