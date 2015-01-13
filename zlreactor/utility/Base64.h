// ***********************************************************************
// Filename         : Base64.h
// Author           : LIZHENG
// Created          : 2014-09-16
// Description      : Base64 编解码
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-09-16
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_BASE64_H
#define ZL_BASE64_H
#include "Define.h"
#include <string>
NAMESPACE_ZL_BASE_START

// len == strlen(src), and strlen(dst) >= len * 4 / 3
// 返回编码后的字符串长度
int         base64Encode(const char *src, int len, char *dst);
int         base64Encode(const char *src, int len, std::string& dst);
int         base64Encode(const std::string& src, std::string& dst);

// len == strlen(src), and strlen(dst) >= len * 3 / 4
// 返回解码后的字符串长度
int         base64Decode(const char *src, size_t len, char *dst);
int         base64Decode(const char *src, int len, std::string& dst);
int         base64Decode(const std::string& src, std::string& dst);

NAMESPACE_ZL_BASE_END
#endif  /* ZL_BASE64_H */