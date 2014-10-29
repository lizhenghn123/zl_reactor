// ***********************************************************************
// Filename         : enable_shared_from_this.h
// Author           : LIZHENG
// Created          : 2014-10-10
// Description      :
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-10
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_ENABLE_SHARED_FROM_THIS_H
#define ZL_ENABLE_SHARED_FROM_THIS_H
#include "smart_ptr/weak_ptr.h"
#include "smart_ptr/shared_ptr.h"

namespace zl
{

template<class T> 
class enable_shared_from_this
{
protected:
    enable_shared_from_this()
    {
    }

    enable_shared_from_this(const enable_shared_from_this &)
    {
    }

    enable_shared_from_this & operator=(const enable_shared_from_this &)
    {
        return *this;
    }

    ~enable_shared_from_this()
    {
    }

public:
    shared_ptr<T> shared_from_this()
    {
        shared_ptr<T> p( weak_this_ );
        assert( p.get() == this );
        return p;
    }

    shared_ptr<T const> shared_from_this() const
    {
        shared_ptr<T const> p( weak_this_ );
        assert( p.get() == this );
        return p;
    }

public: // actually private, but avoids compiler template friendship issues

    // Note: invoked automatically by shared_ptr; do not call
    template<class X, class Y> void _internal_accept_owner( shared_ptr<X> const * ppx, Y * py ) const
    {
        if( weak_this_.expired() )
        {
            weak_this_ = shared_ptr<T>( *ppx, py );
            assert(!weak_this_.expired());
        }
    }

private:
    mutable weak_ptr<T> weak_this_;
};

} // namespace boost

#endif  /* ZL_ENABLE_SHARED_FROM_THIS_H */
