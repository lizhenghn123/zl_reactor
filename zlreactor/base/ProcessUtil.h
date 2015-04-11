// ***********************************************************************
// Filename         : ProcessUtil.h
// Author           : LIZHENG
// Created          : 2015-04-07
// Description      : 进程相关信息获取函数
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-04-08
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_PROCESSUTIL_H
#define ZL_PROCESSUTIL_H
#include "Define.h"
#include "base/Timestamp.h"

#include <string>
using zl::base::Timestamp;
NAMESPACE_ZL_START

namespace ProcessUtil
{
    /// process id of current process.
    pid_t pid();
    string pidString();

    uid_t uid();
    uid_t euid();
    string username();

    /// obtain start time in microseconds
    Timestamp startTime();

    /// obtain elapsed millseconds since start.
    int64_t elapsedTime();

    int clockTicksPerSecond();
    int pageSize();

    string hostname();
    string procname();
    string procname(const string& stat);

    /// read /proc/self/status
    string procStatus();

    /// read /proc/self/stat
    string procStat();

    /// read /proc/self/task/tid/stat
    string threadStat();

    /// readlink /proc/self/exe
    string exePath();

    int openedFiles();
    int maxOpenFiles();

    struct CpuTime
    {
        double userSeconds;
        double systemSeconds;

        CpuTime() : userSeconds(0.0), systemSeconds(0.0) { }
    };
    CpuTime cpuTime();

    int numThreads();
    std::vector<pid_t> threads();
}

using namespace ProcessUtil;
NAMESPACE_ZL_END
#endif /* ZL_PROCESSUTIL_H */