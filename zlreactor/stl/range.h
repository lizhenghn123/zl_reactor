// ***********************************************************************
// Filename         : range.h
// Author           : LIZHENG
// Created          : 2014-06-16
// Description      : 支持整数/浮点数，可以双向迭代
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_STL_RENGE_H
#define ZL_STL_RENGE_H

namespace zl
{
    namespace stl
    {

        template<typename value_t>
        class RangeImpl
        {
            class Iterator;
        public:
            RangeImpl(value_t begin, value_t end, value_t step = 1) :begin_(begin), end_(end), step_(step)
            {
                if (step>0 && begin_ >= end_)
                    throw std::logic_error("end must greater than begin.");
                else if (step<0 && begin_ <= end_)
                    throw std::logic_error("end must less than begin.");

                stepCount_ = (end_ - begin_) / step_;  //计算要遍历多少个元素
                if (begin_ + stepCount_ * step_ != end_)
                {
                    stepCount_++;  //向上取整
                }
            }

            Iterator begin()
            {
                return Iterator(0, *this);
            }

            Iterator end()
            {
                return Iterator(stepCount_, *this);
            }

            value_t operator[](int s)
            {
                return begin_ + s * step_;
            }

            int size()
            {
                return stepCount_;
            }

        private:
            class Iterator
            {
            public:
                Iterator(int start, RangeImpl& range) : currentStep_(start), rangeImpl_(range)
                {
                    currentValue_ = rangeImpl_.begin_ + currentStep_*rangeImpl_.step_;
                }

                value_t operator*() { return currentValue_; }

                const Iterator* operator++()
                {
                    currentValue_ += rangeImpl_.step_;
                    currentStep_++;
                    return this;
                }

                bool operator==(const Iterator& other)
                {
                    return currentStep_ == other.currentStep_;
                }

                bool operator!=(const Iterator& other)
                {
                    return currentStep_ != other.currentStep_;
                }

                const Iterator* operator--()
                {
                    currentValue_ -= rangeImpl_.step_;
                    currentStep_--;
                    return this;
                }

            private:
                value_t     currentValue_;
                int         currentStep_;
                RangeImpl&  rangeImpl_;
            };

        private:
            value_t begin_;      //开始位置
            value_t end_;        //结束位置
            value_t step_;       //步长
            int     stepCount_;  //前进步数
        };


        template<typename T>
        RangeImpl<T> Range(T end)
        {
            return RangeImpl<T>(T(), end, 1);
        }

        template<typename T>
        RangeImpl<T> Range(T begin, T end)
        {
            return RangeImpl<T>(begin, end, 1);
        }

        template<typename T, typename V>
        auto Range(T begin, T end, V stepsize)->RangeImpl<decltype(begin + end + stepsize)>
        {
            return RangeImpl<decltype(begin + end + stepsize)>(begin, end, stepsize);
        }

    }    // end namespace stl
}    // end namespace zl

#endif /* ZL_STL_RENGE_H */
