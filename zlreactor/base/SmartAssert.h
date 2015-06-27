// ***********************************************************************
// Filename         : SmartAssert.h
// Author           : LIZHENG
// Created          : 2014-09-18
// Description      : 增强版的Assert实现
// 
// Last Modified By : LIZHENG
// Last Modified On : 2015-01-11
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SMARTASSERT_H
#define ZL_SMARTASSERT_H
#include <string>
#include <sstream>
#include <stdlib.h>
namespace zl
{
namespace base
{

#define ENABLE_SMART_ASSERT_MODE  //enable ZL_ASSERT macro, use in Debug/Release env 

#define ABORT_IF_ASSERT_FAILED    // if assert failed, abort(), except ZL_ASSERT_LOG

class SmartAssert
{
public:
    SmartAssert(const char* expr, const char* function, int line, const char* file, bool abortOnExit = false)
        : SMART_ASSERT_A(*this),
          SMART_ASSERT_B(*this),
          abortIfExit_(abortOnExit)
    {
        std::ostringstream oss;
        if (expr && *expr)
            oss << "Expression Failed: " << expr << "\n";
        if(function && *function) 
            oss << "Failed in [func: " << function << "], [line: " << line << "], [file: " << file << "]\n";
        errMsg_ += oss.str();
    }

    ~SmartAssert()
    {
        std::cerr << errMsg_ << "\n";

        if(abortIfExit_)
        {
        #if defined(ABORT_IF_ASSERT_FAILED)
            abort();
        #endif
        }
    }

    template< typename T>
    SmartAssert& printValiable(const char* expr, const T& value)
    {
        std::ostringstream oss;
        oss << "ContextValiable: [" << expr << " = " << value << "]\n";
        errMsg_ += oss.str();
        return *this;
    }

public:
    SmartAssert& SMART_ASSERT_A;
    SmartAssert& SMART_ASSERT_B;

private:
    bool  abortIfExit_;
    std::string errMsg_;
};

static SmartAssert MakeAssert(const char* expr, const char* function, int line, const char* file, bool abortOnExit)
{
    return zl::base::SmartAssert(expr, function, line, file, abortOnExit);
}

static SmartAssert __dont_use_this__ = MakeAssert(NULL, NULL, 0, 0, false); //gcc: MakeAssert 定义未使用[-Wunused-function]

// run time assert
#ifndef ENABLE_SMART_ASSERT_MODE
#define ZL_ASSERT(expr)      ((void) 0)
#define ZL_ASSERTEX(expr, func, lineno , file)   ((void) 0)
#define ZL_ASSERT_LOG(expr)  ((void) 0)
#else
#define SMART_ASSERT_A(x)        SMART_ASSERT_OP(x, B)
#define SMART_ASSERT_B(x)        SMART_ASSERT_OP(x, A)
#define SMART_ASSERT_OP(x, next) SMART_ASSERT_A.printValiable(#x, (x)).SMART_ASSERT_##next

#define ZL_ASSERT(expr)          \
            if( (expr) ) ;       \
            else zl::base::MakeAssert(#expr, __FUNCTION__, __LINE__, __FILE__, true).SMART_ASSERT_A
#define ZL_ASSERTEX(expr, func, lineno , file) \
            if( (expr) ) ;                     \
            else zl::base::MakeAssert( #expr, func, lineno, file, true).SMART_ASSERT_A
#define ZL_ASSERT_LOG(expr)       \
            if( (expr) ) ;        \
            else zl::base::MakeAssert(#expr, __FUNCTION__, __LINE__, __FILE__, false).SMART_ASSERT_A
#endif

// compile time assert
#define MACRO_CAT(x, y)     MACRO_DO_CAT(x, y)
#define MACRO_DO_CAT(x, y)  MACRO_DO_CAT2(x, y)
#define MACRO_DO_CAT2(x, y) x##y
#define MACRO_P(x)  #x

#define ZL_STATIC_ASSERT(expr, ...) ZL_STATIC_ASSERT_IMPL(expr, __FILE__, __LINE__)
#define ZL_STATIC_ASSERT_IMPL(expr, file, line)  \
                  typedef char static_assert_fail_on_##file_##line[2 * ((expr) != 0) - 1]

}
}
#endif  /* ZL_SMARTASSERT_H */
