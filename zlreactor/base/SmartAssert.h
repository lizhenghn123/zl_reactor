// ***********************************************************************
// Filename         : SmartAssert.h
// Author           : LIZHENG
// Created          : 2014-09-18
// Description      : 增强版的Assert实现，参考自 http://blog.csdn.net/pongba/article/details/19129
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-09-18
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SMARTASSERT_H
#define ZL_SMARTASSERT_H
namespace zl
{
namespace base
{

#define SMART_ASSERT_DEBUG_MODE  //enable ZL_ASSERT macro, can use in Debug/Release env 

#define ABORT_IF_ASSERT_FAILED

class SmartAssert
{
public:
    SmartAssert(bool abort = false)
        : abortIfExit_(abort),
          SMART_ASSERT_A(*this),
          SMART_ASSERT_B(*this)
    {

    };

    ~SmartAssert()
    {
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
        std::cerr << "PrintValiable: [" << expr << " = " << value << "]\n";
        return *this;
    }

private:
    bool  abortIfExit_;

public:
    SmartAssert& SMART_ASSERT_A;
    SmartAssert& SMART_ASSERT_B;
};

static SmartAssert MakeAssert(bool abort, const char* expr, const char* function, int line, const char* file)
{
    if (expr && *expr)
        std::cerr << "Expression Failed: " << expr << "\n";
    if(function && *function) 
        std::cerr << "Failed in [func: " << function << "], [line: " << line << "], [file: " << file << "]\n";
    return SmartAssert(abort);
}

static SmartAssert __dont_use_this__ = MakeAssert(false, NULL, NULL, 0, 0);  //gcc: MakeAssert 定义未使用[-Wunused-function]

// run time assert
#ifndef SMART_ASSERT_DEBUG_MODE

#define SMART_ASSERT(expr)      ((void) 0)
#define SMART_ASSERT_LOG(expr)  ((void) 0)

#else

#define SMART_ASSERT_A(x)        SMART_ASSERT_OP(x, B)
#define SMART_ASSERT_B(x)        SMART_ASSERT_OP(x, A)
#define SMART_ASSERT_OP(x, next) SMART_ASSERT_A.printValiable(#x, (x)).SMART_ASSERT_##next

#define ZL_ASSERT(expr)          \
            if( (expr) ) ;       \
            else zl::base::MakeAssert(true, #expr, __FUNCTION__, __LINE__, __FILE__).SMART_ASSERT_A

#define ZL_ASSERTEX(expr, func, lineno , file) \
            if( (expr) ) ;                     \
            else zl::base::MakeAssert(true, #expr, func, lineno, file).SMART_ASSERT_A

#define ZL_ASSERT_LOG(expr)       \
            if( (expr) ) ;        \
            else zl::base::MakeAssert(false, #expr, __FUNCTION__, __LINE__, __FILE__).SMART_ASSERT_A

#endif

// compile time assert
#define ZL_STATIC_ASSERT(expr) ZL_STATIC_ASSERT_IMPL(expr, __FILE__, __LINE__)
#define ZL_STATIC_ASSERT_IMPL(expr, file, line)  \
                typedef char static_assert_fail_on_##file_and_##line[2 * ((expr) != 0) - 1]

}
}
#endif  /* ZL_SMARTASSERT_H */
