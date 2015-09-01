// ***********************************************************************
// Filename         : ProcessUtil.h
// Author           : LIZHENG
// Created          : 2015-04-07
// Description      : 进程相关信息获取函数
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_PROCESSUTIL_H
#define ZL_PROCESSUTIL_H
#include "Define.h"
#include <unistd.h>
#include <string>
#include "base/Timestamp.h"

using zl::base::Timestamp;
#ifdef OS_WINDOWS
typedef unsigned long pid_t;
#endif

NAMESPACE_ZL_START

namespace ProcessUtil
{
    uid_t uid();
    uid_t euid();
    string username();
    int clockTicksPerSecond();
    int pageSize();
    string hostname();

    /// 当前进程的信息
    /// 当前进程的id及程序名
    pid_t  pid();
    string pidString();
    string procname();
    string procname(const string& stat);

    /// 获取当前进程的启动时间（毫秒级）
    Timestamp startTime();

    /// 获取当前进程已运行的时间（当前时间减去启动时间, 微秒）
    int64_t elapsedTime();

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

    /// 设置是否允许当前进程生成coredump文件
    /// enabled = true, 启用codedump; core_file_size < 0 , 不限制coredump文件大小
    /// return true iff set success, false otherwise
    bool enableCoreDump(bool enabled = true, int core_file_size = -1);


    /// 获取第三方进程的信息
    /// 根据进程名查找pid, 返回-1表示没找到，否则返回值>0
    int getPidByName(const char* procname);

    /// 根据pid查找进程名, 返回空表示没找到，否则返回非空字符串
    string getNameByPid(pid_t pid);

    /// 获取指定进程的status信息, read /proc/pid/status 
    string procStatus(pid_t pid);

    /// 获取指定进程的stat信息, read /proc/pid/stat 
    string procStat(pid_t pid);

    /// 获取指定进程所创建的线程总数
    int numThreads(pid_t pid);

    /// 获取指定进程所创建的线程tid
    std::vector<pid_t> threads(pid_t pid);

    /// 获取指定进程的执行路径
    string exePath(pid_t pid);
}

using namespace ProcessUtil;
NAMESPACE_ZL_END
#endif /* ZL_PROCESSUTIL_H */