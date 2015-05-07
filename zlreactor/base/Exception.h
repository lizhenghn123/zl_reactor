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
    explicit Exception(const char *errinfo);
    Exception(const char *filename, int linenumber, const char *errinfo);
    Exception(const char *filename, int linenumber, const std::string& errinfo);
    virtual ~Exception() throw();

    virtual const char *what() const throw()
    {
        return errmsg_.c_str();
    }
    
    const char* stack_trace() const throw()
    {
        return callStack_.c_str();
    }

    const char* filename() const throw()
    {
        return filename_.c_str();
    }

    int line() const throw()
    {
        return line_;
    }
private:
    void trace_stack();

    int line_;
    std::string filename_;
    std::string errmsg_;
    std::string callStack_;
};

NAMESPACE_ZL_BASE_END
#endif  /* */
