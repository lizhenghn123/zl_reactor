// ***********************************************************************
// Filename         : UriUtil.h
// Author           : LIZHENG
// Created          : 2015
// Description      : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_HTTP_URIUTIL_H
#define ZL_HTTP_URIUTIL_H
#include "Define.h"
#include <string>
NAMESPACE_ZL_NET_START

size_t      uriEncode(const char* unencoded, size_t len, char* encoded);
std::string uriEncode(const char* unencoded, size_t len);
std::string uriEncode(const std::string& unencoded);


size_t      uriDecode(const char* encoded, size_t len, char* dst);
std::string uriDecode(const char* encoded, size_t len);
std::string uriDecode(const std::string& encoded);

NAMESPACE_ZL_NET_END
#endif /* ZL_HTTP_URIUTIL_H */
