// ***********************************************************************
// Filename         : smart_ptr.h
// Author           : LIZHENG
// Created          : 2014-10-10
// Description      :
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-10
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SMART_PTR_H
#define ZL_SMART_PTR_H

#include "smart_ptr/unique_ptr.h"
#include "smart_ptr/shared_ptr.h"
#include "smart_ptr/weak_ptr.h"
#include "smart_ptr/enable_shared_from_this.h"

namespace zl
{
    template<typename T, typename deleter = default_deleter<T> >
    class scoped_ptr;

    template<typename T>
    class shared_ptr;

    template<typename T>
    class weak_ptr;

    template<class T> 
    class enable_shared_from_this;
}

#endif  /* ZL_SMART_PTR_H */