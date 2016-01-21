// ***********************************************************************
// Filename         : OsDefine.h
// Author           : LIZHENG
// Created          : 2014-07-01
// Description      : 平台定义
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_OSDEFINE_H
#define ZL_OSDEFINE_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define OS_WINDOWS
    #define OS_X86
#elif defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
    #define OS_WINDOWS
    #define OS_X64
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
    #define OS_CYGWIN
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define OS_LINUX
    #if defined(__x86_64__)
        #define OS_X64
    #else
        #define OS_X86
    #endif
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #define OS_MACOSX
#elif defined(__FreeBSD__)
    #define OS_FREEBSD
#elif defined(__NetBSD__)
    #define OS_NETBSD
#elif defined(__OpenBSD__)
    #define OS_OPENBSD
#else
    #error  "You Must Be Choose One Platform"
#endif

#if defined(OS_WINDOWS)
    #if (_MSC_VER >= 1300) && (_MSC_VER < 1400)   // Visual Studio 2003, MSVC++ 7.1
        #define ZL_MSVS_VERSION 2003
        #define ZL_MSVC_VERSION 71
    #elif (_MSC_VER >= 1400) && (_MSC_VER < 1500) // Visual Studio 2005, MSVC++ 8.0
        #define ZL_MSVS_VERSION 2005
        #define ZL_MSVC_VERSION 80
    #elif (_MSC_VER >= 1500) && (_MSC_VER < 1600) // Visual Studio 2008, MSVC++ 9.0
        #define ZL_MSVS_VERSION 2008
        #define ZL_MSVC_VERSION 90
    #elif (_MSC_VER >= 1600) && (_MSC_VER < 1700) // Visual Studio 2010, MSVC++ 10.0
        #define ZL_MSVS_VERSION 2010
        #define ZL_MSVC_VERSION 100
    #elif (_MSC_VER >= 1700) && (_MSC_VER < 1800) // Visual Studio 2012, MSVC++ 11.0
        #define ZL_MSVS_VERSION 2012
        #define ZL_MSVC_VERSION 110
    #endif
#endif

#endif /* ZL_OSDEFINE_H */
