// ***********************************************************************
// Filename         : array.h
// Author           : LIZHENG
// Created          : 2015-03-03
// Description      :
//
// Last Modified By : LIZHENG
// Last Modified On : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_STL_ARRAY_H
#define ZL_STL_ARRAY_H
#include <algorithm>

namespace zl
{
    namespace stl
    {
        /// fixed-size array of elements of type T
        template < typename T, size_t N>
        class array
        {
        public:    /// type definitions
            typedef const array<T, N>& const_this_base_ref;
            typedef array<T, N>        this_base;

            typedef T                  value_type;
            typedef T*                 pointer;
            typedef const T*           const_pointer;
            typedef T&                 reference;
            typedef const T&           const_reference;
            typedef std::ptrdiff_t     difference_type;
            typedef std::size_t        size_type;
            typedef pointer            iterator;
            typedef const_pointer      const_iterator;
            typedef std::reverse_iterator<iterator> reverse_iterator;
            typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        public:
            /// iterator support
            iterator begin()                                { return elems_; }
            iterator end()                                  { return elems_ + N; }
            const_iterator begin() const                    { return elems_; }
            const_iterator end()   const                    { return elems_ + N; }

            // reverse iterator support
            reverse_iterator rbegin()                       { return reverse_iterator(end()); }
            reverse_iterator rend()                         { return reverse_iterator(begin()); }
            const_reverse_iterator rbegin()  const          { return const_reverse_iterator(end()); }
            const_reverse_iterator rend()    const          { return const_reverse_iterator(begin()); }

            /// front() and back()
            reference  front()                              { return elems_[0]; }
            const_reference  front()         const          { return elems_[0]; }
            reference  back()                               { return elems_[N - 1]; }
            const_reference  back()          const          { return elems_[N - 1]; }

            /// operator[]
            reference        operator[] (size_type i)       { return elems_[i]; }
            const_reference  operator[] (size_type i)const  { return elems_[i]; }

            /// at() with range check
            reference at(size_type i)                       { rangecheck(i); return elems_[i]; }
            const_reference at(size_type i)  const          { rangecheck(i); return elems_[i]; }

            size_type size()                 const          { return N; }
            size_type max_size()             const          { return size(); }
            bool empty()                     const          { return false; }

            /// direct access to data (read-only)
            T* data()                                       { return elems_; }
            const T* data()                 const           { return elems_; }

            /// use array as C array (direct read/write access to data)
            T* c_array()                                    { return elems_; }

            /// assign one value to all elements
            void assign(const T& value)                     { fill(value); } 
            void fill(const T& value)
            {
                std::fill_n(begin(), size(), value);
            }

            void swap(this_base y)
            {
                for (size_type i = 0; i < N; ++i)
                    std::swap(elems_[i], y.elems_[i]);
            }

            /// assignment with type conversion
            template <typename T2>
            array<T, N>& operator= (const array<T2, N>& rhs)
            {
                std::copy(rhs.begin(), rhs.end(), begin());
                return *this;
            }

        private:
            void rangecheck(size_type i)
            {
                if (i >= size())
                {
                    std::out_of_range e("array<>: index out of range");
                    throw e;
                }
            }
        private:
            T elems_[N];
        };

        /// not implement when N == 0
        template < typename T>
        class array < T, 0 > ;

        /// comparisons
        template<class T, std::size_t N>
        bool operator== (const array<T, N>& x, const array<T, N>& y)
        {
            return std::equal(x.begin(), x.end(), y.begin());
        }
        template<class T, std::size_t N>
        bool operator< (const array<T, N>& x, const array<T, N>& y)
        {
            return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
        }
        template<class T, std::size_t N>
        bool operator!= (const array<T, N>& x, const array<T, N>& y)
        {
            return !(x == y);
        }
        template<class T, std::size_t N>
        bool operator> (const array<T, N>& x, const array<T, N>& y)
        {
            return y<x;
        }
        template<class T, std::size_t N>
        bool operator<= (const array<T, N>& x, const array<T, N>& y)
        {
            return !(y<x);
        }
        template<class T, std::size_t N>
        bool operator>= (const array<T, N>& x, const array<T, N>& y)
        {
            return !(x<y);
        }

        /// global swap()
        template<class T, std::size_t N>
        inline void swap(array<T, N>& x, array<T, N>& y)
        {
            x.swap(y);
        }
    }
}

#endif  /* ZL_STL_ARRAY_H */