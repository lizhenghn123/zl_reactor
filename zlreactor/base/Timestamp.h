// ***********************************************************************
// Filename         : Timestamp.h
// Author           : LIZHENG
// Created          : 2014-10-21
// Description      :
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-21
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_TIMESTAMP_H
#define ZL_TIMESTAMP_H
#include <string>

#define ZL_MSEC_PER_SEC   (1000)
#define ZL_USEC_PER_SEC   (1000 * 1000)
#define ZL_TIME_SEC(time) ((time) / ZL_USEC_PER_SEC)

namespace zl{
namespace base {

typedef long long zl_time_t;

class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(zl_time_t ms);
public:
    static Timestamp now();
public:
    zl_time_t micoSeconds() const
    {
        return microSeconds_;
    }
    zl_time_t millSeconds() const
    {
        return microSeconds_ / ZL_MSEC_PER_SEC;
    }
    zl_time_t seconds()     const
    {
        return microSeconds_ / ZL_USEC_PER_SEC;
    }

    struct tm  *getTm(bool showlocaltime = true);
    std::string toString(bool showlocaltime = true);

private:
    zl_time_t  microSeconds_;
};

inline bool operator<(const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.micoSeconds() < rhs.micoSeconds();
}

inline bool operator==(const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.micoSeconds() == rhs.micoSeconds();
}

} // namespace base
} // namespace zl
#endif  /* ZL_TIMESTAMP_H */