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
#include "Define.h"
#include <string>

#define ZL_MSEC_PER_SEC   (1000)
#define ZL_USEC_PER_SEC   (1000 * 1000)
#define ZL_TIME_SEC(time) ((time) / ZL_USEC_PER_SEC)

namespace zl{
namespace base {

class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(int64_t ms);
public:
    static Timestamp now();
    static Timestamp invalid();

    static double  timediff(const Timestamp& lhs, const Timestamp& rhs)
    {
         int64_t delta = lhs.microSeconds() - rhs.microSeconds();
         return ZL_TIME_SEC(delta * 1.0);
    }
public:
    int64_t microSeconds() const
    {
        return microSeconds_;
    }
    int64_t millSeconds()  const
    {
        return microSeconds_ / ZL_MSEC_PER_SEC;
    }
    int64_t seconds()      const
    {
        return microSeconds_ / ZL_USEC_PER_SEC;
    }

    bool valid() const
    {
        return microSeconds_ > 0;
    }

    struct tm  *getTm(bool showlocaltime = true);
    std::string toString(bool showlocaltime = true);

private:
    int64_t  microSeconds_;
};

inline bool operator<(const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.microSeconds() < rhs.microSeconds();
}

inline bool operator==(const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.microSeconds() == rhs.microSeconds();
}

inline Timestamp operator+(const Timestamp& lhs, double seconds)
{
    long delta = seconds * ZL_USEC_PER_SEC;
    return Timestamp(lhs.microSeconds() + delta);
}

inline Timestamp operator+(double seconds, const Timestamp& rhs)
{
    return (rhs + seconds);
}

inline Timestamp operator+=(Timestamp& lhs, double seconds)
{
    return lhs = lhs + seconds;
}

} // namespace base
} // namespace zl
#endif  /* ZL_TIMESTAMP_H */