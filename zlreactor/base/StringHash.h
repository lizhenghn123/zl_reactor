// ***********************************************************************
// Filename         : StringHash.h
// Author           : LIZHENG
// Created          : 2014-09-23
// Description      : 字符串的多种hash计算
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-09-23
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_STRINGHASH_H
#define ZL_STRINGHASH_H
#include "Define.h"
NAMESPACE_ZL_BASE_START

//SDBM Hash Function
unsigned int SDBMHash(const char *str);

//RS Hash Function
unsigned int RSHash(const char *str);

//JS Hash Function
unsigned int JSHash(const char *str);

//BKDR Hash Function
unsigned int BKDRHash(const char *str);

//DJB Hash Function
unsigned int DJBHash(const char *str);

//FNV Hash Function
unsigned int FNVHash(const char *str);

NAMESPACE_ZL_BASE_END
#endif  /* ZL_STRINGHASH_H */