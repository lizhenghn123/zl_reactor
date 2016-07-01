// ***********************************************************************
// Filename         : StopWatch.h
// Author           : LIZHENG
// Created          : 2014-04-28
// Description      : 高精度计时器
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_STOPWTACH_H
#define ZL_STOPWTACH_H
#include "zlreactor/Define.h"
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

NAMESPACE_ZL_BASE_START

#define GET_TICK_COUNT(a, b) ((a.tv_sec - b.tv_sec)*1000000 + (a.tv_usec - b.tv_usec))

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
    static struct timeval  now()
    {
        struct timeval now;
        getTimeOfDay(&now, NULL);
        return now;
    }
    float   elapsedTime()
    {
        struct timeval now;
        getTimeOfDay(&now, NULL);
        return float(GET_TICK_COUNT(now, start_time) / 1000000.0);
    }
    float   elapsedTimeInMill()
    {
        struct timeval now;
        getTimeOfDay(&now, NULL);
        return float(GET_TICK_COUNT(now, start_time) / 1000.0);
    }
    int64_t   elapsedTimeInMicro()
    {
        timeval now;
        getTimeOfDay(&now, NULL);
        return GET_TICK_COUNT(now, start_time);
    }
    float   diffTime(const struct timeval& start)
    {
        struct timeval now;
        getTimeOfDay(&now, NULL);
        return float(GET_TICK_COUNT(now, start) / 1000000.0);
    }
    float   diffTime(const struct timeval& start, const struct timeval& end)
    {
        return float(GET_TICK_COUNT(end, start) / 1000000.0);
    }
private:
    void start()
    {
        reset();
    }
    static void getTimeOfDay(struct timeval *tv, void *tz)
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
    #endif
    }
private:
    struct timeval      start_time;
};

NAMESPACE_ZL_BASE_END
#endif /** ZL_STOPWTACH_H */
