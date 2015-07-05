// ***********************************************************************
// Filename         : Date.h
// Author           : LIZHENG
// Created          : 2014-09-17
// Description      : 日期类
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_DATE_H
#define ZL_DATE_H
#include <string>

namespace zl{
namespace base {
//#define IsLeapYear(y) ((!(y % 4)) ? (((!(y % 400)) && (y % 100)) ? 1 : 0) : 0)

class Date
{
public:
    Date();
    Date(int year, int month, int day);

public:
    static Date today();
    static Date utcToday();
    static bool isLeapYear(int year);
    static bool isValid(int year, int month, int day);
    static int  daysInMonth(int year, int month);
    static int  daysInPreviousMonth(int year, int month);
    static int  daysInNextMonth(int year, int month);
    static int  compare(const Date &lhs, const Date &rhs);
    static int  daysDiff(const Date &from, const Date &to);

public:
    bool set(int year, int month, int day);
    bool year(int year);
    bool month(int month);
    bool day(int day);

    int  year()
    {
        return year_;
    }
    int  month()
    {
        return month_;
    }
    int  day()
    {
        return day_;
    }
    bool isLeapYear()
    {
        return isLeapYear(year_);
    }

    Date &addYears(int years);
    Date &addMonths(int months);
    Date &addDays(int days);

    Date nextDay() const;
    Date nextMonth() const;
    Date nextYear() const;

    bool isEqual(const Date &rhs) const;

    int  daysDiff(const Date &to) const; // 距离日期to有多少天
    int  daysToNextYear() const;         // 距离下一年的今天有多少天
    int  daysToNextMonth() const;        // 距离下个月的今天有多少天
    int  daysToPreviousYear() const;     // 距离上一年的今天有多少天
    int  daysToPreviousMonth() const;    // 距离上个月的今天有多少天

    std::string toString() const;

public:
    Date &operator+=(int days);
    Date &operator-=(int days);
    Date &operator++();
    Date &operator--();
    Date  operator++(int);
    Date  operator--(int);
    Date  operator+(int days);
    Date  operator-(int days);

    bool operator<(const Date &rhs) const
    {
        return Date::compare(*this, rhs) < 0;
    }

    bool operator>(const Date &rhs) const
    {
        return Date::compare(*this, rhs) > 0;
    }

    bool operator==(const Date &rhs) const
    {
        return Date::compare(*this, rhs) == 0;
    }

    bool operator<=(const Date &rhs) const
    {
        return Date::compare(*this, rhs) <= 0;
    }

    bool operator>=(const Date &rhs) const
    {
        return Date::compare(*this, rhs) >= 0;
    }

    bool operator!=(const Date &rhs) const
    {
        return Date::compare(*this, rhs) != 0;
    }
    //	friend bool operator<(const Date& lhs, const Date& rhs) const;
private:
    void adjustMonth();

private:
    int year_;    // [1900..]
    int month_;   // [1..12]
    int day_;     // [1..31]
};

} // namespace base
} // namespace zl
#endif  /* ZL_DATE_H */
