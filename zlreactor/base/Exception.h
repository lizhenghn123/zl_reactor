// ***********************************************************************
// Filename         : Exception.h
// Author           : LIZHENG
// Created          : 2014-11-05
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-11-05
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_EXCEPTION_H
#define ZL_EXCEPTION_H
#include "Define.h"
#include <exception>
NAMESPACE_ZL_BASE_START

class Exception : public std::exception
{
public:
    explicit Exception(const char* errinfo);
    explicit Exception(const std::string& errinfo);
    virtual ~Exception() throw();

    virtual const char* what() const throw()
    {
        return errmsg_.c_str();
    }

    const char* stack_trace() const throw()
    {
        return callStack_.c_str();
    }

private:
    void trace_stack();

    std::string errmsg_;
    std::string callStack_;
};

NAMESPACE_ZL_BASE_END
#endif  /* */
