// ***********************************************************************
// Filename         : any.h
// Author           : LIZHENG
// Created          : 2015-03-04
// Description      :
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-01-18
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_STL_ANY_H
#define ZL_STL_ANY_H

namespace zl
{
    namespace stl
    {
        class any
        {
        public:
            any() : content_(0)
            {
            }

            any(const any& that) : content_(that.clone())
            {
            }

            any(any && that) : content_(std::move(that.content_))
            {
            }

            //创建智能指针时，对于一般的类型，通过std::decay来移除引用和cv符，从而获取原始类型
            template<typename U>
            any(U && value) : content_(new holder <typename std::decay<U>::type>(std::forward<U>(value)))
            {
            }

            any& operator=(const any& rhs)
            {
                any(rhs).swap(*this);
                return *this;
            }

            template<typename ValueType>
            any & operator=(const ValueType & rhs)
            {
                any(rhs).swap(*this);
                return *this;
            }

            ~any()
            {
                delete content_;
            }

        public:
            any& swap(any & rhs)
            {
                std::swap(content_, rhs.content_);
                return *this;
            }

            bool empty() const
            {
                return !content_;
            }

            void clear()
            {
                any().swap(*this);
            }

            const std::type_info & type() const
            {
                return content_ ? content_->type() : typeid(void);
            }

        private:
            struct placeholder
            {
                virtual ~placeholder() { }
                virtual const std::type_info & type() const = 0;
                virtual placeholder * clone() const = 0;
            };

            template <typename ValueType>
            class holder : public placeholder
            {
            public:
                holder(const ValueType & value) : held_(value)
                {
                }
            public:
                virtual const std::type_info & type() const
                {
                    return typeid(ValueType);
                }

                virtual placeholder * clone() const
                {
                    return new holder(held_);
                }
            public:
                ValueType held_;
            private:
                holder & operator=(const holder &);
            };

            placeholder* clone() const
            {
                if (content_ != 0)
                    return content_->clone();

                return 0;
            }

        private:
            template<typename ValueType>
            friend ValueType* any_cast(any *);

        private:
            placeholder *content_;
        };

        inline void swap(any & lhs, any & rhs)
        {
            lhs.swap(rhs);
        }

        class bad_any_cast :  public std::bad_cast
        {
        public:
            virtual const char * what() const throw()
            {
                return "zl::bad_any_cast: failed conversion using zl::any_cast";
            }
        };

        template<typename ValueType>
        inline ValueType* any_cast(any * operand)
        {
            //any::holder<ValueType>* derived = static_cast<any::holder<ValueType>*> (operand->content_);
            //return &derived->held_;
            return  operand && operand->type() == typeid(ValueType)
                //? &static_cast<any::holder<std::remove_cv<ValueType>::type> *>(operand->content_)->held_
			    ? &static_cast<any::holder<ValueType>*>(operand->content_)->held_
                : 0;
        }

        template<typename ValueType>
        inline const ValueType* any_cast(const any * operand)
        {
            return zl::stl::any_cast<ValueType>(const_cast<any *>(operand));
        }

        template<typename ValueType>
        inline ValueType any_cast(any & operand)
        {
            //typedef std::remove_reference<ValueType>::type nonref;
            typedef ValueType nonref;

            nonref * result = any_cast<nonref>(&operand);
            if(!result)
                throw bad_any_cast();

            return static_cast<ValueType>(*result);
        }

        template<typename ValueType>
        inline ValueType any_cast(const any & operand)
        {
            //typedef std::remove_reference<ValueType>::type nonref;
			typedef ValueType nonref;
            return any_cast<const nonref &>(const_cast<any &>(operand));
        }

    }
}
#endif  /* ZL_STL_ANY_H */