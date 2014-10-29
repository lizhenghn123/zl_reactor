// ***********************************************************************
// Filename         : StlUtil.h
// Author           : LIZHENG
// Created          : 2014-10-17
// Description      : stl库相关工具函数
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-17
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_STLUTIL_H
#define ZL_STLUTIL_H
#include "Define.h"
#include <sstream>
NAMESPACE_ZL_START

/** 使用一对迭代器，删除每个迭代器所指向的指针 */
template <typename ForwardIterator>
void stlDeleteContainerPointer(ForwardIterator begin, ForwardIterator end)
{
    while (begin != end)
    {
        ForwardIterator tmp = begin;
        ++begin;
        Safe_Delete(*tmp);
    }
}

/** 删除某一容器包含的指针 */
template <class T>
void stlDeleteElems(T &container)
{
    if (container.empty())
        return;
    stlDeleteContainerPointer(container.begin(), container.end());
    container.clear();
}

/** 删除某一容器包含的指针 */
template <class T>
void stlDeleteElems(T *container)
{
    if (!container)
        return;
    stlDeleteContainerPointer(container->begin(), container->end());
    container->clear();
}

/** 删除关联容器中的value值（指针） */
template <class T>
void stlDeleteValues(T &v)
{
    if (v.empty())
        return;
    for (typename T::iterator i = v.begin(); i != v.end(); ++i)
    {
        Safe_Delete(i->second);
    }
    v.clear();
}

/** 删除关联容器中的value值（指针） */
template <class T>
void stlDeleteValues(T *v)
{
    if (!v)
        return;
    for (typename T::iterator i = v->begin(); i != v->end(); ++i)
    {
        Safe_Delete(i->second);
    }
    v->clear();
}

/** 仿函数：删除map/multimap/hash_map中的value指针，可用于for_each等算法中 */
template<typename TKEY, typename TVALUE>
struct del_map_second_ptr
{
public :
    template<typename TKEY, typename TVALUE>
    void operator()(std::pair<TKEY, TVALUE *> p)
    {
        Safe_Delete(p.second);
    }
};

NAMESPACE_ZL_END
#endif  /* ZL_STLUTIL_H */