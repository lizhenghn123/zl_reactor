// ***********************************************************************
// Filename         : Define.h
// Author           : LIZHENG
// Created          : 2014-07-01
// Description      :
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-07-01
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_STDDEFINE_H
#define ZL_STDDEFINE_H
#include <vector>
#include <string>
#include <list>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <stdint.h>
#include <assert.h>
#include "OsDefine.h"
#ifdef OS_WINDOWS
#define  _WINSOCKAPI_
#include <Windows.h>
#endif

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus > 199711L || __cplusplus == 201103L
#define ZL_CXX11_ENABLED 1
#endif

#define NAMESPACE_ZL_START        namespace zl {
#define NAMESPACE_ZL_END          }  /* namespace zl */

#define NAMESPACE_ZL_BASE_START   NAMESPACE_ZL_START namespace base {
#define NAMESPACE_ZL_BASE_END     } }  /* namespace zl::base */

#define NAMESPACE_ZL_THREAD_START NAMESPACE_ZL_START namespace thread {
#define NAMESPACE_ZL_THREAD_END   } }  /* namespace zl::thread */

#define NAMESPACE_ZL_NET_START    NAMESPACE_ZL_START namespace net {
#define NAMESPACE_ZL_NET_END      } }  /* namespace zl::net */

#define NAMESPACE_ZL_UTIL_START   NAMESPACE_ZL_START namespace util {
#define NAMESPACE_ZL_UTIL_END     } }  /* namespace zl::util */


#ifdef OS_WINDOWS
#define ZL_SNPRINTF  _snprintf
#else
#define ZL_SNPRINTF  snprintf
#endif

#define Safe_Delete(p)        do { delete p; p = NULL; } while (0)
#define Safe_Delete_Array(p)  do { delete[] p; p = NULL; } while (0)

#ifdef ZL_CXX11_ENABLED
#define DISALLOW_COPY_AND_ASSIGN(TypeName)            \
        TypeName(const TypeName&) = delete;           \
        TypeName& operator=(const TypeName&) = delete
#else
#define DISALLOW_COPY_AND_ASSIGN(TypeName)            \
        private:                                      \
            TypeName(const TypeName&);                \
            TypeName& operator=(const TypeName&)      
#endif

// Run-time assertion
// #define ZL_NDEBUG
#ifdef  ZL_NDEBUG
#define ZL_ASSERT(expr) ((void) 0)
#define ZL_ASSERTEX(expr, file, lineno, func) ((void) 0)
#else
#define ZL_ASSERT(expr)                                    \
			((void) ((expr) ? 0 :                          \
		    printf("%s:%d: %s:  Assertion `%s' failed.\n", \
			__FILE__, __LINE__, __FUNCTION__, #expr)))
#define ZL_ASSERTEX(expr, file, lineno, func)              \
	        ((void) ((expr) ? 0 :                          \
		    printf("%s:%d: %s: Assertion `%s' failed. "    \
		    "(called from %s:%d:%s)\n",                    \
		     __FILE__, __LINE__, __FUNCTION__, #expr,      \
            file, lineno, func)))
#endif

// Compile-time assertion
#define ZL_STATIC_ASSERT(expr) ZL_STATIC_ASSERT_IMPL(expr, __FILE__, __LINE__)
#define ZL_STATIC_ASSERT_IMPL(expr, file, line) typedef char static_assert_fail_on_##file_and_##line[2*((expr)!=0)-1]


#endif /* ZL_STDDEFINE_H */