// ***********************************************************************
// Filename         : Daemonize.h
// Author           : LIZHENG
// Created          : 2015-01-05
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-01-05
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_DAEMONIZE_H
#define ZL_DAEMONIZE_H
namespace zl
{
namespace base
{

int createDaemonize(const char *pidfile, int nochdir = 1, int noclose = 0);

int exitDaemonize(const char *pidfile);

}
}
#endif  /* ZL_DAEMONIZE_H */