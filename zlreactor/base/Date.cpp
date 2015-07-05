#include "base/Date.h"
#include "Define.h"
#include <stdio.h>
#include <time.h>

namespace zl{
namespace base {

static int const gDaysInMonth[] =
{
    // 1   2   3   4   5   6   7   8   9   10  11  12
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

Date::Date()
{
    set(1970, 1, 1);
}

Date::Date(int year, int month, int day)
{
    set(year, month, day);
}

bool Date::set(int year, int month, int day)
{
    if (!isValid(year, month, day))
        return false;
    year_ = year;
    month_ = month;
    day_ = day;
    return true;
}

bool Date::year(int year)
{
    year_ = year;
    adjustMonth();
    return true;
}

bool Date::month(int month)
{
    if (month <= 0 || month_ > 12)
        return false;
    month_ = month;
    adjustMonth();
    return true;
}

bool Date::day(int day)
{
    if (day <= 0 || day > 31)
        return false;
    if (day > daysInMonth(year_, month_))
        return false;
    day_ = day;
    return true;
}

Date &Date::addYears(int years)
{
    year(year_ + years);
    return *this;
}

Date &Date::addMonths(int months)
{
    if (months == 0)
        return *this;

    if (months > 0)
    {
        int years = months / 12;
        months %= 12;
        year_ += years;
        month_ += months;
        if (month_ > 12)
        {
            ++year_;
            month_ -= 12;
        }
    }
    else
    {
        months = -months;
        int years = months / 12;
        months %= 12;
        year_ -= years;
        month_ -= months;
        if (month_ < 1)
        {
            --year_;
            month_ += 12;
        }
    }

    adjustMonth();
    return *this;
}

Date &Date::addDays(int days)
{
    if (days == 0)
        return *this;

    if (days > 0)
    {
        for (;;) // Add years one by one.
        {
            int days_to_next_year = daysToNextYear();
            if (days < days_to_next_year)
                break;
            addYears(1);
            days -= days_to_next_year;
        }
        for (;;) // Add months one by one.
        {
            int days_to_next_month = daysToNextMonth();
            if (days < days_to_next_month)
                break;
            addMonths(1);
            days -= days_to_next_month;
        }
        day_ = day_ + days;
        int days_in_month = daysInMonth(year_, month_);
        if (day_ > days_in_month) // Month overflow, adjust to next month.
        {
            day_ -= days_in_month;
            addMonths(1);
        }
    }
    else
    {
        days = -days;
        for (;;)
        {
            int days_to_previous_year = daysToPreviousYear();
            if (days < days_to_previous_year)
                break;
            addYears(-1);
            days -= days_to_previous_year;
        }
        for (;;)
        {
            int days_to_previous_month = daysToPreviousMonth();
            if (days < days_to_previous_month)
                break;
            addMonths(-1);
            days -= days_to_previous_month;
        }
        day_ = day_ - days;
        if (day_ < 1)
        {
            int days_in_prev_month = daysInPreviousMonth(year_, month_);
            day_ += days_in_prev_month;
            addMonths(-1);
        }
    }

    return *this;
}

void Date::adjustMonth()
{
    if (month_ == 2 && day_ > 28)
    {
        int days = daysInMonth(year_, month_);
        if (day_ > days)
            day_ = days;
    }
}

int Date::daysInPreviousMonth(int year, int month)
{
    return month > 1 ? daysInMonth(year, month - 1) : daysInMonth(year - 1, 12);
}

int Date::daysInNextMonth(int year, int month)
{
    return month < 12 ? daysInMonth(year, month + 1) : daysInMonth(year + 1, 1);
}

Date Date::nextMonth() const
{
    Date result(*this);
    return result.addMonths(1);
}

Date Date::nextYear() const
{
    Date result(*this);
    return result.addYears(1);
}

Date &Date::operator+=(int days)
{
    addDays(days);
    return *this;
}

Date &Date::operator-=(int days)
{
    addDays(-days);
    return *this;
}

Date &Date::operator++()
{
    ++day_;
    if (day_ > daysInMonth(year_, month_))
    {
        day_ = 1;
        ++month_;
        if (month_ > 12)
        {
            ++year_;
            month_ = 1;
        }
    }
    return *this;
}

Date &Date::operator--()
{
    --day_;
    if (day_ == 0)
    {
        --month_;
        if (month_ == 0)
        {
            month_ = 12;
            --year_;
        }
        day_ = daysInMonth(year_, month_);
    }
    return *this;
}

Date Date::operator++(int)
{
    Date org(*this);
    ++*this;
    return org;
}

Date Date::operator--(int)
{
    Date org(*this);
    --*this;
    return org;
}

Date Date::operator+(int days)
{
    Date newd(*this);
    newd += days;
    return newd;
}
Date Date::operator-(int days)
{
    Date newd(*this);
    newd -= days;
    return newd;
}

bool  Date::isEqual(const Date &rhs) const
{
    return year_ == rhs.year_ && month_ == rhs.month_ && day_ == rhs.day_;
}

int Date::compare(const Date &lhs, const Date &rhs)
{
    if (lhs.year_ > rhs.year_)
        return 1;
    if (lhs.year_ < rhs.year_)
        return -1;
    if (lhs.month_ > rhs.month_)
        return 1;
    if (lhs.month_ < rhs.month_)
        return -1;
    if (lhs.day_ > rhs.day_)
        return 1;
    if (lhs.day_ < rhs.day_)
        return -1;
    return 0;
}

int  Date::daysDiff(const Date &from, const Date &to)
{
    return from.daysDiff(to);
}

int Date::daysDiff(const Date &to) const
{
    if (to < *this)
        return to.daysDiff(*this);
    int days = 0;
    Date from(*this);
    for (;;)
    {
        Date next_year = from.nextYear();
        if (next_year > to)
            break;
        days += from.daysToNextYear();
        from = next_year;
    }
    for (;;)
    {
        Date next_month = from.nextMonth();
        if (next_month > to)
            break;
        days += from.daysToNextMonth();
        from = next_month;
    }
    while (from < to)
    {
        ++from;
        ++days;
    }
    return days;
}

int Date::daysToNextYear() const
{
    if (month_ > 2 || (month_ == 2 && day_ > 28))
    {
        return 365 + isLeapYear(year_ + 1);
    }
    else
    {
        return 365 + isLeapYear(year_);
    }
}

int Date::daysToNextMonth() const
{
    int this_month_days = daysInMonth(year_, month_);
    int next_month_days = daysInNextMonth(year_, month_);
    if (day_ <= next_month_days)
        return this_month_days;
    return next_month_days + this_month_days - day_;
}

int Date::daysToPreviousYear() const
{
    if (month_ < 2 || (month_ == 2 && day_ < 28))
    {
        return 365 + isLeapYear(year_ - 1);
    }
    else
    {
        return 365 + isLeapYear(year_);
    }
}

int Date::daysToPreviousMonth() const
{
    int prev_month_days = daysInPreviousMonth(year_, month_);
    int day = static_cast<int>(day_);
    return day > prev_month_days ? day : prev_month_days;
}

std::string Date::toString() const
{
    char buf[16];
    ZL_SNPRINTF(buf, 16, "%04d-%02d-%02d", year_, month_, day_);
    return buf;
}

bool Date::isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

bool Date::isValid(int year, int month, int day)
{
    if (month < 1 || month > 12)
        return false;
    if (day < 1 || day > daysInMonth(year, month))
        return false;
    return true;
}

int Date::daysInMonth(int year, int month)
{
    int days = gDaysInMonth[month];
    if (month == 2 && isLeapYear(year))
        ++days;
    return days;
}

Date Date::today()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t); // NOLINT(runtime/threadsafe_fn)
    return Date(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

Date Date::utcToday()
{
    time_t t = time(NULL);
    struct tm *tm = gmtime(&t); // NOLINT(runtime/threadsafe_fn)
    return Date(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

} // namespace base
} // namespace zl
