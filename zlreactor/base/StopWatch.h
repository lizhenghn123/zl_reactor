// ***********************************************************************
// Filename         : StopWatch.h
// Author           : LIZHENG
// Created          : 2014-04-28
// Description      : 高精度计时器
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-05-14
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_STOPWTACH_H
#define ZL_STOPWTACH_H
#include "Define.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#include <time.h>
//struct timeval
//{
//	long tv_sec, tv_usec;
//};
#elif defined(OS_LINUX)
#include <sys/time.h>
#else
#error "You must be include OsDefine.h firstly"
#endif

NAMESPACE_ZL_START

#define GET_TICK_COUNT(a, b) ((b.tv_sec - a.tv_sec)*1000000 + (b.tv_usec - a.tv_usec))

class StopWatch
{
public:
    StopWatch()
    {
        start();
    }
public:
    void     reset()
    {
        getTimeOfDay(&start_time, NULL);
    }
    static timeval  now()
    {
        timeval now;
        getTimeOfDay(&now, NULL);
        return now;
    }
    float   elapsedTime()
    {
        timeval now;
        getTimeOfDay(&now, NULL);
        return float(GET_TICK_COUNT(now, start_time) / 1000000.0);
    }
    float   elapsedTimeInMill()
    {
        timeval now;
        getTimeOfDay(&now, NULL);
        return float(GET_TICK_COUNT(now, start_time) / 1000.0);
    }
    float   elapsedTimeInMicro()
    {
        timeval now;
        getTimeOfDay(&now, NULL);
        return float(GET_TICK_COUNT(now, start_time));
    }
    float   diffTime(const timeval& start)
    {
        timeval now;
        getTimeOfDay(&now, NULL);
        return float(GET_TICK_COUNT(now, start) / 1000000.0);
    }
    float   diffTime(const timeval& start, const timeval& end)
    {
        return float(GET_TICK_COUNT(end, start) / 1000000.0);
    }
private:
    void start()
    {
        reset();
    }
    static void getTimeOfDay(timeval *tv, void *tz)
    {
#ifdef OS_LINUX
        gettimeofday(tv, NULL);
#elif defined(OS_WINDOWS)
        typedef unsigned __int64 uint64;
#define EPOCHFILETIME (116444736000000000ULL)
        FILETIME ft;
        LARGE_INTEGER li;
        uint64 tt;

        GetSystemTimeAsFileTime(&ft);
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        tt = (li.QuadPart - EPOCHFILETIME) / 10;
        tv->tv_sec = tt / 1000000;
        tv->tv_usec = tt % 1000000;
        ////以下计算精度不够
        //time_t clock;
        //struct tm tm;
        //SYSTEMTIME wtm;
        //
        //GetLocalTime(&wtm);
        //tm.tm_year     = wtm.wYear - 1900;
        //tm.tm_mon     = wtm.wMonth - 1;
        //tm.tm_mday     = wtm.wDay;
        //tm.tm_hour     = wtm.wHour;
        //tm.tm_min     = wtm.wMinute;
        //tm.tm_sec     = wtm.wSecond;
        //tm. tm_isdst    = -1;
        //clock = mktime(&tm);
        //tv->tv_sec = clock;
        //tv->tv_usec = wtm.wMilliseconds * 1000;
#endif
    }
private:
    timeval		start_time;
};

NAMESPACE_ZL_END

#endif /** ZL_STOPWTACH_H */
