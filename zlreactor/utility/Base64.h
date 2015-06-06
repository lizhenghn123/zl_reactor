// ***********************************************************************
// Filename         : Base64.h
// Author           : LIZHENG
// Created          : 2014-09-16
// Description      : Base64 编解码
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-06-04
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_BASE64_H
#define ZL_BASE64_H
#include "Define.h"
#include <string>
NAMESPACE_ZL_UTIL_START

// len == strlen(src), and strlen(dst) >= len * 4 / 3
// 返回编码后的字符串长度
size_t         base64Encode(const char *src, size_t len, char *dst);
size_t         base64Encode(const char *src, size_t len, std::string& dst);
size_t         base64Encode(const std::string& src, std::string& dst);
std::string    base64Encode(const char *src, size_t len);
std::string    base64Encode(const std::string& src);

// len == strlen(src), and strlen(dst) >= len * 3 / 4
// 返回解码后的字符串长度
size_t         base64Decode(const char *src, size_t len, char *dst);
size_t         base64Decode(const char *src, size_t len, std::string& dst);
size_t         base64Decode(const std::string& src, std::string& dst);
std::string    base64Decode(const char *src, size_t len);
std::string    base64Decode(const std::string& src);

NAMESPACE_ZL_UTIL_END
#endif  /* ZL_BASE64_H */