// ***********************************************************************
// Filename         : NonCopy.h
// Author           : LIZHENG
// Created          : 2014-05-16
// Description      : 禁止拷贝构造、赋值构造，建议private继承
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-05-16
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_NONCOPY_H
#define ZL_NONCOPY_H

namespace zl
{
    class NonCopy
    {
    protected:
        NonCopy() {}
        ~NonCopy() {}
    private:
        NonCopy(const NonCopy&);
        const NonCopy& operator=(const NonCopy&);
    };
}

#endif /* ZL_NONCOPY_H */