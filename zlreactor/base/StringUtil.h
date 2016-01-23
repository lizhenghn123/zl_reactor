// ***********************************************************************
// Filename         : StringUtil.h
// Author           : LIZHENG
// Created          : 2014-08-16
// Description      : 字符串辅助函数
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_STRING_UTIL_H
#define ZL_STRING_UTIL_H
#include <string.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
namespace zl{
namespace base{


/// 格式化字符串
size_t stringFormatAppend(std::string *dst, const char* format, ...);
size_t stringFormat(std::string *dst, const char* format, ...);
std::string stringFormat(const char *format, ...);

/// 任意类型转为字符串
template <typename T>
inline std::string toStr(const T& t)
{
    std::ostringstream oss;
    oss << t;
    return oss.str();
}

/// 字符串转为某一类型
template <typename T>
T strTo(const std::string& str)
{
    T t;
    std::istringstream iss(str);
    iss >> t;
    return t;
}

/// 字符串忽略大小写比较, 可用作容器（比如map、set）的比较子
struct string_cmp_nocase : public std::binary_function<std::string, std::string, bool>
{
public:
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
        std::string::const_iterator p = lhs.begin();
        std::string::const_iterator p2 = rhs.begin();

        while (p != lhs.end() && p2 != rhs.end())
        {
            if (toupper(*p) != toupper(*p2))
                return (toupper(*p) < toupper(*p2) ? 1 : 0);
            ++p;
            ++p2;
        }

        return (lhs.size() == rhs.size()) ? 0 : (lhs.size() < rhs.size()) ? 1 : 0;
    }
};

/// 将字符串转为小写并返回
inline std::string toLower(const std::string& str)
{
    std::string t = str;
    std::transform(t.begin(), t.end(), t.begin(), ::tolower);
    return t;
}

/// 将字符串转为小写并返回
inline std::string toUpper(const std::string& str)
{
    std::string t = str;
    std::transform(t.begin(), t.end(), t.begin(), ::toupper);
    return t;
}

/// 判断字符串是否以某一子串为开始
inline bool startsWith(const std::string& str, const std::string& substr)
{
    return str.find(substr) == 0;
}

/// 判断字符串是否以某一子串为结尾
inline bool endsWith(const std::string& str, const std::string& substr)
{
    return str.rfind(substr) == (str.length() - substr.length());
}

/// 比较两个字符串是否相等
inline bool equals(const std::string& lhs, const std::string& rhs)
{
    return (lhs) == (rhs);
}

/// 去掉字符串中左边属于字符串delim中任一字符的所有字符(默认去除空格)
inline std::string trimLeft(std::string& str, const char* delim = " ")
{
    str.erase(0, str.find_first_not_of(delim));
    return str;
}

/// 去掉字符串中右边属于字符串delim中任一字符的所有字符(默认去除空格)
inline std::string trimRight(std::string& str, const char* delim = " ")
{
    str.erase(str.find_last_not_of(delim) + 1);
    return str;
}

/// 去掉字符串中两端属于字符串delim中任一字符的所有字符(默认去除空格)
inline std::string trim(std::string& str, const char* delim = " ")
{
    trimLeft(str, delim);
    trimRight(str, delim);
    return str;
}

/// 去掉字符串中的所有特定单一字符
inline std::string erase(std::string& str, char c = ' ')
{
    str.erase(std::remove_if(str.begin(), str.end(), std::bind2nd(std::equal_to<char>(), c)), str.end());
    return str;
}

/// 字符串替换 去掉字符串中的某特定字符串delim并以新字符串s代替
inline std::string replaceAll(std::string& str, const char* delim, const char* s = "")
{
    size_t len = strlen(delim);
    size_t pos = str.find(delim);
    while (pos != std::string::npos)
    {
        str.replace(pos, len, s);
        pos = str.find(delim, pos);
    }
    return str;
}

/// 字符串分隔，insertEmpty : 如果有连续的delim，是否插入空串
inline void split(const std::string& str, std::vector<std::string>& result,
    const std::string& delim = " ", bool insertEmpty = false)
{
    if(str.empty() || delim.empty())
        return;

    std::string::const_iterator substart = str.begin(), subend;
    while(true)
    {
        subend = std::search(substart, str.end(), delim.begin(), delim.end());
        std::string temp(substart, subend);

        if(!temp.empty())
        {
            result.push_back(temp);
        }
        else if(insertEmpty)
        {
            result.push_back("");
        }

        if(subend == str.end())
            break;
        substart = subend + delim.size();
    }
}

/// 字符串合并
template< typename SequenceSequenceT, typename Range1T>
inline typename SequenceSequenceT::value_type join(const SequenceSequenceT& Input, const Range1T& Separator)
{
    typedef typename SequenceSequenceT::value_type ResultT;
    typedef typename SequenceSequenceT::const_iterator InputIteratorT;

    InputIteratorT itBegin = Input.begin();
    InputIteratorT itEnd = Input.end();

    ResultT Result;

    if(itBegin != itEnd)
    {
        Result += *itBegin;
        ++itBegin;
    }

    for(; itBegin != itEnd; ++itBegin)
    {
        Result += Separator;    // Add separator
        Result += *itBegin;     // Add element
    }

    return Result;
}

template<typename SequenceSequenceT, typename Range1T, typename PredicateT>
inline typename SequenceSequenceT::value_type 
    join_if(const SequenceSequenceT& Input, const Range1T& Separator, PredicateT Pred)
{
    typedef typename SequenceSequenceT::value_type ResultT;
    typedef typename SequenceSequenceT::const_iterator InputIteratorT;

    InputIteratorT itBegin = Input.begin();
    InputIteratorT itEnd = Input.end();

    ResultT Result;

    while(itBegin!=itEnd && !Pred(*itBegin)) ++itBegin;

    if(itBegin != itEnd)
    {
        Result += *itBegin;
        ++itBegin;
    }

    for(; itBegin != itEnd; ++itBegin)
    {
        if(Pred(*itBegin))
        {
            Result += Separator;    // Add separator
            Result += *itBegin;     // Add element
        }
    }

    return Result;
}

} // namespace base
} // namespace zl
#endif  // ZL_STRING_UTIL_H
