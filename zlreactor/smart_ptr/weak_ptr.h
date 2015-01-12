// ***********************************************************************
// Filename         : weak_ptr.h
// Author           : LIZHENG
// Created          : 2014-10-10
// Description      :
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-10
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_WAEK_PTR_H
#define ZL_WAEK_PTR_H
#include "shared_ptr.h"
namespace zl
{
    /************************************************
    template<typename T> class weak_ptr
    ************************************************/
    template<typename T>
    class weak_ptr
    {
    public:
        typedef T element_type;

    public:
        weak_ptr() : ptr_(0), weak_node(0)
        {
        }

        weak_ptr(const weak_ptr& wp)
        {
            ptr_ = wp.ptr_;
            weak_node = wp.weak_node;
            if(weak_node)
                weak_node->ref_count += 1;
        }

        template<typename Y>
        weak_ptr(const weak_ptr<Y>& wp)
        {
            ptr_ = wp.ptr_;
            weak_node = wp.weak_node;
            if(weak_node)
                weak_node->ref_count += 1;
        }

        template<typename Y>
        weak_ptr(const shared_ptr<Y>& sp)
        {
            ptr_ = sp.ptr_;
            if(sp.shared_node)
            {
                if(sp.shared_node->weak_node)
                {
                    weak_node = sp.shared_node->weak_node;
                    weak_node->ref_count += 1;
                }
                else
                {
                    weak_node = new weak_ptr_node(sp.shared_node);
                    sp.shared_node->weak_node = weak_node;
                }
            }
            else
            {
                weak_node = 0;
            }
        }

        weak_ptr& operator= (const weak_ptr& wp)
        {
            weak_ptr(wp).swap(*this);
            return *this;
        }

        template<typename Y>
        weak_ptr& operator= (const weak_ptr<Y>& wp)
        {
            weak_ptr(wp).swap(*this);
            return *this;
        }

        template<typename Y>
        weak_ptr& operator=(const shared_ptr<Y>& sp)
        {
            weak_ptr(sp).swap(*this);
            return *this;
        }

        ~weak_ptr()
        {
            if(weak_node)
            {
                weak_node->ref_count -= 1;

                if(weak_node->ref_count == 0)
                {
                    if(expired() == false)
                        weak_node->shared_node->weak_node = 0;
                    delete weak_node; weak_node = 0;
                }
            }
        }

    public:
        long use_count() const
        {
            if(expired())
                return 0;
            else
                return weak_node->shared_node->ref_count;
        }

        bool expired() const
        {
            return weak_node == 0 || weak_node->shared_node == 0;
        }

        void reset()
        {
            weak_ptr().swap(*this);
        }

        shared_ptr<T> lock() const
        {
            if(expired())
                return shared_ptr<T>();
            else
                return shared_ptr<T>(*this);
        }

        void swap(weak_ptr<T>& wp)
        {
            std::swap(ptr_, wp.ptr_);
            std::swap(weak_node, wp.weak_node);
        }

    private:
        template <typename Y> friend class shared_ptr;
        template <typename Y> friend class weak_ptr;

    private:
        T *ptr_;
        weak_ptr_node *weak_node;
    };


    template<typename T>
    void swap(weak_ptr<T>& lhs, weak_ptr<T>& rhs)
    {
        lhs.swap(rhs);
    }

} /* namespace zl */
#endif  /* ZL_WAEK_PTR_H */