// ***********************************************************************
// Filename         : shared_ptr.h
// Author           : LIZHENG
// Created          : 2014-10-10
// Description      :
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-10
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SHARED_PTR_H
#define ZL_SHARED_PTR_H
#include <typeinfo>
#include <exception>
#include <assert.h>

namespace zl
{
    template<typename T> class weak_ptr;
    template<typename T> class shared_ptr;
    template<typename T> class enable_shared_from_this;

    class bad_weak_ptr: public std::exception {};

    struct shared_ptr_deleter
    {
        virtual void destory(const void *p) = 0;
        virtual ~shared_ptr_deleter() {}

        virtual void* get_deleter(const std::type_info& t) const = 0;
    };

    struct shared_ptr_node;
    struct weak_ptr_node
    {
        weak_ptr_node(shared_ptr_node *sn) : ref_count(1), shared_node(sn)
        {
            //assert(sn && "weak_ptr_node(shared_ptr_node *sn): sp can not be null");
        }

        long ref_count;
        shared_ptr_node *shared_node;
    };

    struct shared_ptr_node
    {
        shared_ptr_node() :	ref_count(1), deleter(0), weak_node(0)
        {}

        long ref_count;
        shared_ptr_deleter *deleter;
        weak_ptr_node *weak_node;
    };

    struct shared_ptr_static_cast {};
    struct shared_ptr_dynamic_cast {};
    struct shared_ptr_const_cast {};

    // enable_shared_from_this support
    namespace detail
    {
        template< class X, class Y> 
        inline void sp_enable_shared_from_this(zl::shared_ptr<X> const * ppx, Y const * py, zl::enable_shared_from_this<Y> const * pe)
        {
            if( pe != 0 )
            {
                pe->_internal_accept_owner( ppx, const_cast< Y* >( py ) );
            }
        }

        inline void sp_enable_shared_from_this( ... )
        {
        }
    }
    /************************************************
    template<typename T> class shared_ptr
    ************************************************/
    template<typename T>
    class shared_ptr
    {
        struct default_deleter : public shared_ptr_deleter
        {
            void destory(const void *ptr)
            {
                T *p = (T *)ptr;
                delete p;
                p = NULL;
            }

            virtual void *get_deleter(const std::type_info&) const
            {
                return 0;
            }
        };

        template <typename D>
        struct deleter_template : public shared_ptr_deleter
        {
            deleter_template(const D& d_) : d(d_) {}

            void destory(const void *p)
            {
                d((T *)p);
            }
            
            virtual void *get_deleter(const std::type_info& t) const
            {
                if(typeid(D) == t)
                    return (void *)&d;
                else
                    return 0;
            }
            
            D d;
        };

    public:
        typedef T element_type;

    public:
        shared_ptr() : ptr_(0), shared_node(0)
        {

        }

        shared_ptr(const shared_ptr& r)
        {
            ptr_ = r.ptr_;
            shared_node = r.shared_node;
            if(shared_node)
                shared_node->ref_count += 1;
        }

        template<typename Y>
        explicit shared_ptr(Y *p) : ptr_(p)
        {
            try
            {
                shared_node = new shared_ptr_node;
                shared_node->deleter = new default_deleter;
            }
            catch(...)
            {
                delete shared_node;
                throw;
            }
            zl::detail::sp_enable_shared_from_this(this, p, p);
        }

        template<typename Y>
        explicit shared_ptr(const shared_ptr<Y>& p, T *t) : ptr_(t), shared_node(p.shared_node)
        {
            if(shared_node)  
                shared_node->ref_count += 1;
        }

        template<typename Y, typename D>
        shared_ptr(Y *p, const D& d) :	ptr_(p)
        {
            assert(p && "shared_ptr::shared_ptr(p,d): p can't be null");
            try
            {
                shared_node = 0;
                shared_node = new shared_ptr_node;
                shared_node->del = new deleter_template<D>(d);
            }
            catch(...)
            {
                if(shared_node)
                    delete shared_node;
                d(p);
                throw;
            }
        }

        template<typename Y>
        shared_ptr(const shared_ptr<Y>& r)
        {
            ptr_ = r.ptr_;
            shared_node = r.shared_node;
            if(shared_node)
                shared_node->ref_count += 1;
        }

        template<typename Y>
        shared_ptr(const shared_ptr<Y>& sp, const shared_ptr_static_cast&)
        {
            ptr_ = static_cast<T*>(sp.ptr_);
            if(ptr_ != 0)
            {
                shared_node = sp.shared_node;
                shared_node->ref_count += 1;
            }
            else
            {
                shared_node = 0;
            }
        }

        template<typename Y>
        shared_ptr(const shared_ptr<Y>& sp, const shared_ptr_dynamic_cast&)
        {
            ptr_ = dynamic_cast<T *>(sp.ptr_);
            if(ptr_ != 0)
            {
                shared_node = sp.shared_node;
                shared_node->ref_count += 1;
            }
            else
            {
                shared_node = 0;
            }
        }

        template<typename Y>
        shared_ptr(const shared_ptr<Y>& sp, const shared_ptr_const_cast&)
        {
            ptr_ = const_cast<T *>(sp.ptr_);
            if(ptr_ != 0)
            {
                shared_node = sp.shared_node;
                shared_node->ref_count += 1;
            }
            else
            {
                shared_node = 0;
            }
        }

        template<typename Y>
        explicit shared_ptr(const weak_ptr<Y>& wp)
        {
            if(wp.expired())
                throw bad_weak_ptr();

            ptr_ = wp.ptr_;
            shared_node = wp.weak_node->shared_node;
            shared_node->ref_count += 1;
        }

        shared_ptr& operator=(const shared_ptr& rhs)
        {
            shared_ptr(rhs).swap(*this);
            return *this;
        }

        template<typename Y>
        shared_ptr& operator= (const shared_ptr<Y>& rhs)
        {
            shared_ptr(rhs).swap(*this);
            return *this;
        }

        ~shared_ptr()
        {
            if(shared_node != 0)
            {
                if(shared_node->ref_count == 1)
                {
                    shared_node->deleter->destory(ptr_);
                    delete shared_node->deleter;

                    if(shared_node->weak_node)
                        shared_node->weak_node->shared_node = 0;

                    delete shared_node;
                    shared_node = 0;
                }
                else
                {
                    shared_node->ref_count -= 1;
                }
            }
        }

        template <typename D>
        D *_get_deleter() const
        {
            if(shared_node && shared_node->del)
                return static_cast<D *>(shared_node->del->get_deleter(typeid(D)));
            else
                return 0;
        }

    public:
        T& operator*() const
        {
            assert(ptr_ && "shared_ptr::operator*(): ptr can not be null");
            return *ptr_;
        }

        T *operator->() const
        {
            assert(ptr_ && "shared_ptr::operator->(): ptr can not be null");
            return ptr_;
        }

        T *get() const
        {
            return ptr_;
        }

        void reset()
        {
            shared_ptr().swap(*this);
        }

        template<typename Y>
        void reset(Y *p)
        {
            assert(p && "shared_ptr::reset(p) : p can not be null");
            shared_ptr(p).swap(*this);
        }

        template<typename Y, typename D>
        void reset(Y *p, const D& d)
        {
            assert(p && "shared_ptr::reset(p,d) : p can not be null");
            shared_ptr(p, d).swap(*this);
        }

        bool unique() const
        {
            return use_count() == 1;
        }

        long use_count() const
        {
            if(shared_node != 0)
                return shared_node->ref_count;
            else
                return 0;
        }

        operator bool() const
        {
            return ptr_ != 0;
        }

        void swap(shared_ptr& b)
        {
            std::swap(data, b.data);
            std::swap(shared_node, b.shared_node);
        }

    private:
        template <typename Y> friend class shared_ptr;
        template <typename Y> friend class weak_ptr;

    private:
        T *ptr_;
        shared_ptr_node *shared_node;
    };


    template<typename T, typename Y>
    bool operator== (const shared_ptr<T>& lhs, const shared_ptr<Y>& rhs)
    {
        return lhs.get() == rhs.get();
    }

    template<typename T, typename Y>
    bool operator!= (const shared_ptr<T>& lhs, const shared_ptr<Y>& rhs)
    {
        return lhs.get() != rhs.get();
    }

    template<typename T>
    void swap(shared_ptr<T>& lhs, shared_ptr<T>& rhs)
    {
        lhs.swap(rhs);
    }

    template<typename To, typename From>
    shared_ptr<To> static_pointer_cast(const shared_ptr<From>& sp)
    {
        return shared_ptr<To>(sp, shared_ptr_static_cast());
    }

    template<typename To, typename From>
    shared_ptr<To> dynamic_pointer_cast(const shared_ptr<From>& sp)
    {
        return shared_ptr<To>(sp, shared_ptr_dynamic_cast());
    }

    template<typename To, typename From>
    shared_ptr<To> const_pointer_cast(const shared_ptr<From>& sp)
    {
        return shared_ptr<To>(sp, shared_ptr_const_cast());
    }

    template<typename E, typename T, typename Y>
    std::basic_ostream<E, T>& operator<< (std::basic_ostream<E, T>& os, const shared_ptr<Y>& sp)
    {
        os << sp.get();
        return os;
    }

    template<typename D, typename T>
    D *get_deleter(const shared_ptr<T>& sp)
    {
        return sp.template _get_deleter<D>();
    }

} /* namespace zl */
#endif  /* ZL_SHARED_PTR_H */