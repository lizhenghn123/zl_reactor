// ***********************************************************************
// Filename         : OsDefine.h
// Author           : LIZHENG
// Created          : 2014-07-01
// Description      : 平台定义
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-06-21
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_OSDEFINE_H
#define ZL_OSDEFINE_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
# define OS_WINDOWS
# define OS_X86
#elif defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
# define OS_WINDOWS
# define OS_X64
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
# define OS_CYGWIN
#elif defined(linux) || defined(__linux) || defined(__linux__)
# define OS_LINUX
#if defined(__x86_64__)
# define OS_X64
#else
# define OS_X86
#endif
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
# define OS_MACOSX
#elif defined(__FreeBSD__)
# define OS_FREEBSD
#elif defined(__NetBSD__)
# define OS_NETBSD
#elif defined(__OpenBSD__)
# define OS_OPENBSD
#else
#error  "You Must Be Choose One Platform"
#endif

#endif /* ZL_OSDEFINE_H */