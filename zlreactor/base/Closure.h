// ***********************************************************************
// Filename         : Closure.h
// Author           : LIZHENG
// Created          : 2014-10-17
// Description      :
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-23
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_FILEUTIL_H
#define ZL_FILEUTIL_H
#include "Define.h"
NAMESPACE_ZL_START

class Closure
{
    DISALLOW_COPY_AND_ASSIGN(Closure);
public:
    Closure(bool self_delete = true) 
        : self_delete_(self_delete)
    {}

    virtual ~Closure() {}

    virtual void run() = 0;

protected:
    bool self_delete_;
};

namespace internal
{
    class FunctionClosure0 : public Closure
    {
    public:
        typedef void (*FunctionType)();

        FunctionClosure0(FunctionType function, bool self_delete)
            : Closure(self_delete), function_(function)
        {}

        ~FunctionClosure0() {}

        void run()
        {
            function_();
            if (self_delete_) delete this;
        }

    private:
        FunctionType function_;
    };

    template <class Class>
    class MethodClosure0 : public Closure
    {
    public:
        typedef void (Class::*MethodType)();

        MethodClosure0(Class *object, MethodType method, bool self_delete)
            : Closure(self_delete), object_(object), method_(method)
        {}

        ~MethodClosure0() {}

        void run()
        {
            (object_->*method_)();
            if (self_delete_) delete this;
        }

    private:
        Class *object_;
        MethodType method_;
    };

    template <typename Arg1>
    class FunctionClosure1 : public Closure
    {
    public:
        typedef void (*FunctionType)(const Arg1& arg1);

        FunctionClosure1(FunctionType function, const Arg1& arg1, bool self_delete)
            : Closure(self_delete), function_(function), arg1_(arg1)
        {}

        ~FunctionClosure1() {}

        void run()
        {
            function_(arg1_);
            if (self_delete_) delete this;
        }

    private:
        FunctionType function_;
        Arg1 arg1_;
    };

    template <class Class, typename Arg1>
    class MethodClosure1 : public Closure
    {
    public:
        typedef void (Class::*MethodType)(const Arg1& arg1);

        MethodClosure1(Class *object, MethodType method, const Arg1& arg1, bool self_delete)
            : Closure(self_delete), object_(object), method_(method), arg1_(arg1)
        {}

        ~MethodClosure1() {}

        void run()
        {
            (object_->*method_)(arg1_);
            if (self_delete_) delete this;
        }

    private:
        Class *object_;
        MethodType method_;
        Arg1 arg1_;
    };

    template <typename Arg1, typename Arg2>
    class FunctionClosure2 : public Closure
    {
    public:
        typedef void (*FunctionType)(const Arg1& arg1, const Arg2& arg2);

        FunctionClosure2(FunctionType function, const Arg1& arg1, const Arg2& arg2, bool self_delete)
            : Closure(self_delete), function_(function), arg1_(arg1), arg2_(arg2)
        {}

        ~FunctionClosure2() {}

        void run()
        {
            function_(arg1_, arg2_);
            if (self_delete_) delete this;
        }

    private:
        FunctionType function_;
        Arg1 arg1_;
        Arg2 arg2_;
    };

    template <class Class, typename Arg1, typename Arg2>
    class MethodClosure2 : public Closure
    {
    public:
        typedef void (Class::*MethodType)(const Arg1& arg1, const Arg2& arg2);

        MethodClosure2(Class *object, MethodType method, const Arg1& arg1, const Arg2& arg2, bool self_delete)
            : Closure(self_delete), object_(object), method_(method), arg1_(arg1), arg2_(arg2)
        {}

        ~MethodClosure2() {}

        void run()
        {
            (object_->*method_)(arg1_, arg2_);
            if (self_delete_) delete this;
        }

    private:
        Class *object_;
        MethodType method_;
        Arg1 arg1_;
        Arg2 arg2_;
    };

    template <typename Arg1, typename Arg2, typename Arg3>
    class FunctionClosure3 : public Closure
    {
    public:
        typedef void (*FunctionType)(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3);

        FunctionClosure3(FunctionType function, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, bool self_delete)
            : Closure(self_delete), function_(function), arg1_(arg1), arg2_(arg2), arg3_(arg3)
        {}

        ~FunctionClosure3() {}

        void run()
        {
            function_(arg1_, arg2_, arg3_);
            if (self_delete_) delete this;
        }

    private:
        FunctionType function_;
        Arg1 arg1_;
        Arg2 arg2_;
        Arg3 arg3_;
    };

    template <class Class, typename Arg1, typename Arg2, typename Arg2>
    class MethodClosure3 : public Closure
    {
    public:
        typedef void (Class::*MethodType)(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3);

        MethodClosure3(Class *object, MethodType method, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, bool self_delete)
            : Closure(self_delete), object_(object), method_(method), arg1_(arg1), arg2_(arg2), arg3_(arg3)
        {}

        ~MethodClosure3() {}

        void run()
        {
            (object_->*method_)(arg1_, arg2_, arg3_);
            if (self_delete_) delete this;
        }

    private:
        Class *object_;
        MethodType method_;
        Arg1 arg1_;
        Arg2 arg2_;
        Arg3 arg3_;
    };
}  // namespace internal

inline Closure* GenCallback(void (*function)())
{
    return new internal::FunctionClosure0(function, true);
}

inline Closure* GenPermanentCallback(void (*function)())
{
    return new internal::FunctionClosure0(function, false);
}

template <class Class>
inline Closure* GenCallback(Class *object, void (Class::*method)())
{
    return new internal::MethodClosure0<Class>(object, method, true);
}

template <class Class>
inline Closure* GenPermanentCallback(Class *object, void (Class::*method)())
{
    return new internal::MethodClosure0<Class>(object, method, false);
}

template <typename Arg1>
inline Closure* GenCallback(void (*function)(Arg1), const Arg1& arg1)
{
    return new internal::FunctionClosure1<Arg1>(function, arg1, true);
}

template <typename Arg1>
inline Closure* GenPermanentCallback(void (*function)(Arg1), const Arg1& arg1)
{
    return new internal::FunctionClosure1<Arg1>(function, arg1, false);
}

template <class Class, typename Arg1>
inline Closure* GenCallback(Class *object, void (Class::*method)(Arg1), const Arg1& arg1)
{
    return new internal::MethodClosure1<Class, Arg1>(object, method, arg1, true);
}

template <class Class, typename Arg1>
inline Closure* GenPermanentCallback(Class *object, void (Class::*method)(Arg1), const Arg1& arg1)
{
    return new internal::MethodClosure1<Class, Arg1>(object, method, arg1, false);
}

template <typename Arg1, typename Arg2>
inline Closure* GenCallback(void (*function)(Arg1, Arg2), const Arg1& arg1, const Arg2& arg2)
{
    return new internal::FunctionClosure2<Arg1, Arg2>(function, arg1, arg2, true);
}

template <typename Arg1, typename Arg2>
inline Closure* GenPermanentCallback(void (*function)(Arg1, Arg2), const Arg1& arg1, const Arg2& arg2)
{
    return new internal::FunctionClosure2<Arg1, Arg2>(function, arg1, arg2, false);
}

template <class Class, typename Arg1, typename Arg2>
inline Closure* GenCallback(Class *object, void (Class::*method)(Arg1, Arg2), const Arg1& arg1, const Arg2& arg2)
{
    return new internal::MethodClosure2<Class, Arg1, Arg2>(object, method, arg1, arg2, true);
}

template <class Class, typename Arg1, typename Arg2>
inline Closure* GenPermanentCallback(Class *object, void (Class::*method)(Arg1, Arg2), const Arg1& arg1, const Arg2& arg2)
{
    return new internal::MethodClosure2<Class, Arg1, Arg2>(object, method, arg1, arg2, false);
}

template <typename Arg1, typename Arg2, typename Arg3>
inline Closure* GenCallback(void (*function)(Arg1, Arg2, Arg3), const Arg1& arg1, const Arg2& arg2, const Arg3& arg3)
{
    return new internal::FunctionClosure2<Arg1, Arg2>(function, arg1, arg2, arg3, true);
}

template <typename Arg1, typename Arg2, typename Arg3>
inline Closure* GenPermanentCallback(void (*function)(Arg1, Arg2, Arg3), const Arg1& arg1, const Arg2& arg2, const Arg3& arg3)
{
    return new internal::FunctionClosure2<Arg1, Arg2>(function, arg1, arg2, arg3, false);
}

template <class Class, typename Arg1, typename Arg2, typename Arg3>
inline Closure* GenCallback(Class *object, void (Class::*method)(Arg1, Arg2, Arg3), const Arg1& arg1, const Arg2& arg2, const Arg3& arg3)
{
    return new internal::MethodClosure2<Class, Arg1, Arg2>(object, method, arg1, arg2, arg3, true);
}

template <class Class, typename Arg1, typename Arg2, typename Arg3>
inline Closure* GenPermanentCallback(Class *object, void (Class::*method)(Arg1, Arg2, Arg3), const Arg1& arg1, const Arg2& arg2, const Arg3& arg3)
{
    return new internal::MethodClosure2<Class, Arg1, Arg2>(object, method, arg1, arg2, arg3, false);
}

NAMESPACE_ZL_END
#endif /* ZL_FILEUTIL_H */