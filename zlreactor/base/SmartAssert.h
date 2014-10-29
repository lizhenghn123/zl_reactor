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
#include <stdio.h>

#define SMART_ASSERT_DEBUG_MODE  //enable SMART_ASSERT macro, can use in Debug/Release env 

class SmartAssert
{
public:
	SmartAssert(bool abort = false): isAbort_(abort), SMART_ASSERT_A(*this), SMART_ASSERT_B(*this)
	{};
	~SmartAssert()
	{
		if(isAbort_)
			abort();
	}
	SmartAssert& SMART_ASSERT_A;
	SmartAssert& SMART_ASSERT_B;

	template< typename T>
	SmartAssert& printValiable(const char* expr, T value)
	{
		std::cout << "PrintValiable: [" << expr << " = " << value << "]\n";
		return *this;
	}
private:
	bool  isAbort_;
};

SmartAssert MakeAssert(bool enAbort, const char* expr, const char* function, const char* file,  int line)
{
	if (expr && *expr)
	{
		printf("expression Failed: %s\n", expr);
	}
	printf("Failed in func %s(line: %d, file: %s)\n", function, line, file);
	if(!enAbort)
		//write some msgs to log
	return SmartAssert (enAbort);
}

#ifdef SMART_ASSERT_DEBUG_MODE
#define SMART_ASSERT_A(x)        SMART_ASSERT_OP(x,B)
#define SMART_ASSERT_B(x)        SMART_ASSERT_OP(x,A)
#define SMART_ASSERT_OP(x, next) SMART_ASSERT_A.printValiable(#x, (x)).SMART_ASSERT_##next

#define SMART_ASSERT(expr)     \
          if( (expr) ) ;       \
          else MakeAssert(false, #expr, __FUNCTION__, __FILE__, __LINE__).SMART_ASSERT_A
#define SMART_ASSERT_LOG(expr) \
	      if( (expr) ) ;       \
		  else MakeAssert(false, #expr, __FUNCTION__, __FILE__, __LINE__).SMART_ASSERT_A
#else
#define SMART_ASSERT(expr)      ((void) 0)
#define SMART_ASSERT_LOG(expr)  ((void) 0)
#endif

#endif  /* ZL_SMARTASSERT_H */