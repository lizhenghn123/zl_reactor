// ***********************************************************************
// Filename         : Daemonize.h
// Author           : LIZHENG
// Created          : 2015-01-05
// Description      : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_DAEMONIZE_H
#define ZL_DAEMONIZE_H
namespace zl
{
    namespace base
    {
        /// 创建守护进程(设置同台机器是否允许多个进程启动)
        /// nochdir : 为0改变当前进程工作目录，否则不改变
        /// noclose : 为0重定向输入输出到/dev/null，否则不会改变, nochdir, noclose 参数意义同 man 3 daemon 参数
        /// pidfile : 为NULL时允许多个进程启动，否则禁止系统重复启动进程
        /// 成功返回当前进程pid，失败返回-1
        int createDaemonize(int nochdir = 1, int noclose = 0, const char *pidfile = 0);

        int exitDaemonize(const char *pidfile);

    }
}
#endif  /* ZL_DAEMONIZE_H */
