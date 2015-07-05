// ***********************************************************************
// Filename         : StlUtil.h
// Author           : LIZHENG
// Created          : 2014-10-17
// Description      : stl辅助工具函数
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_STLUTIL_H
#define ZL_STLUTIL_H
#include "Define.h"
NAMESPACE_ZL_BASE_START

/// 通过swap技巧释放stl容器占用的内存，clear() or reserve() 未必真正释放内存
template<class T>
void clearMemory(T *v)
{
	T tmp;
	tmp.swap(*v);
	v->reserve(0);
}

/// clear内存使用，如果内存超限（limit），转为调用clearMemory
template <class T> 
void clearIfBig(T *v, size_t limit = 1 << 20)
{
	if (v->capacity() >= limit)
		clearMemory(v);
	else
		v->clear();
}

/// 用于将stl vector传递给c接口调用
template<typename T>
inline T* vectorAsArray(std::vector<T> *v)
{ 
# ifdef NDEBUG
	return &*v->begin();
# else   // v 为空时如果直接&*v->begin()会coredump
	return v->empty() ? NULL : &*v->begin();
# endif
}

template<typename T>
inline const T* vectorAsArray(const std::vector<T> *v)
{
# ifdef NDEBUG
	return &*v->begin();
# else
	return v->empty() ? NULL : &*v->begin();
# endif
}

/// 用于将stl string传递给c接口调用
inline char* stringAsArray(std::string* v)
{
    // DO NOT USE const_cast<char*>(v->data())!
    return v->empty() ? NULL : &*v->begin();
}

inline const char* stringAsArray(const std::string* v)
{
    return v->empty() ? NULL : &*v->begin();
}

/// 使用一对迭代器，删除每个迭代器所指向的指针
template <typename ForwardIterator>
void deleteContainerPointer(ForwardIterator begin, ForwardIterator end)
{
    while (begin != end)
    {
        ForwardIterator tmp = begin;
        ++begin;
        Safe_Delete(*tmp);
    }
}

/// 删除关联容器中key指针和value指针
template <class ForwardIterator>
void deleteContainerPairPointer(ForwardIterator begin, ForwardIterator end)
{
	while (begin != end)
    {
		ForwardIterator temp = begin;
		++begin;
		Safe_Delete(temp->first);
        Safe_Delete(temp->second);
	}
}

/// 删除关联容器中key指针
template <class ForwardIterator>
void deleteContainerPairFirstPointer(ForwardIterator begin, ForwardIterator end)
{
	while (begin != end)
    {
		ForwardIterator temp = begin;
		++begin;
		delete (temp->first);
	}
}

/// 删除关联容器中value指针
template <class ForwardIterator>
void deleteContainerPairSecondPointer(ForwardIterator begin, ForwardIterator end)
{
	while (begin != end)
    {
		delete begin->second;
		++begin;
	}
}

/// 删除某一序列式容器包含的指针
template <class Sequence>
void deleteElements(Sequence *v)
{
    if (!v)
        return;
    deleteContainerPointer(v->begin(), v->end());
    v->clear();
}

/// 删除关联容器中的key值(指针)
template <class Container>
void deleteKeys(Container *v)
{
	if (!v || v->empty())
		return;
	//for (typename Container::iterator i = v->begin(); i != v->end(); ++i)
	//{
	//    Safe_Delete(i->first);
	//}
	deleteContainerPairFirstPointer(v->begin(), v->end());
	v->clear();
}

/// 删除关联容器中的value值(指针)
template <class Container>
void deleteValues(Container *v)
{
    if (!v || v->empty())
        return;
	//for (typename Container::iterator i = v->begin(); i != v->end(); ++i)
	//{
	//    Safe_Delete(i->second);
	//}
	deleteContainerPairSecondPointer(v->begin(), v->end());
	v->clear();
}

/// 仿函数：删除list/vector/stack等序列式容器中的指针，可用于 RAII 或 STL算法中
template<typename Sequence>
class ElementDeleter
{
public:
    //template< typename Sequence>
    ElementDeleter<Sequence>(Sequence *ptr) : sequence_(ptr) {}
	~ElementDeleter<Sequence>() { deleteElements(sequence_); }
private:
	Sequence *sequence_;
};

/// 仿函数：删除map/multimap/hash_map等关联式容器中的key指针，可用于 RAII 或 STL算法中
template<typename Container>
class KeyDeleter
{
public:
	KeyDeleter(Container *ptr) : container_(ptr) {}
	~KeyDeleter() { deleteKeys(container_); }
private:
	Container *container_;
};

/// 仿函数：删除map/multimap/hash_map等关联式容器中的value指针，可用于 RAII 或 STL算法中
template<typename Container>
class ValueDeleter
{
public:
	ValueDeleter<Container>(Container *ptr) : container_(ptr) {}
	~ValueDeleter<Container>() { deleteValues(container_); }
private:
	Container *container_;
};

NAMESPACE_ZL_BASE_END
#endif  /* ZL_STLUTIL_H */
