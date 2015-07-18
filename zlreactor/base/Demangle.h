// ***********************************************************************
// Filename         : Demangle.h
// Author           : LIZHENG
// Created          : 2014-11-05
// Description      : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_DEMANGLE_H
#define ZL_DEMANGLE_H
#include "Define.h"
NAMESPACE_ZL_BASE_START

// 根据重整后的名字解析出原函数原型名字
// 如果则返回true, 并将解析后的名字保存在unmangled
// Demangle "mangled".  On success, return true and write the
// demangled symbol name to "unmangled".  Otherwise, return false.
// "unmangled" is modified even if demangling is unsuccessful.
bool demangleName(const char *mangled, char *unmangled, size_t buf_size);

bool demangleName(const char *mangled, std::string& unmangled);

NAMESPACE_ZL_BASE_END
#endif  /* ZL_DEMANGLE_H */
