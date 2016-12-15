#include "zlreactor/base/Daemonize.h"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>     // for kill
#include <errno.h>
#include <stdlib.h>     // for exit
#include <sys/file.h>   // for open, O_RDWR
#include <sys/stat.h>   // for umask
#include <sys/param.h>  // for NOFILE

#define USE_DAEMON_SYSCALL

namespace zl
{
namespace base
{

static int initDaemon(int nochdir = 1, int noclose = 0);
static int checkPid(const char *pidfile);
static int writePid(const char *pidfile);

int createDaemonize(int nochdir/* = 1*/, int noclose/* = 0*/, const char *pidfile/* = 0*/)
{
    int pid = 0;
    if (pidfile)
    {
        int pid = checkPid(pidfile);
        if (pid > 0)
        {
            fprintf(stderr, "The process is already running, pid = %d.\n", pid);
            return -1;
        }
    }

    if (initDaemon(nochdir, noclose) != 0)
    {
        fprintf(stderr, "Now daemonize failed.\n");
        return -1;
    }

    if (pidfile)
    {
        pid = writePid(pidfile);
    }
    else
    {
        pid = ::getpid();
    }
    return pid;
}

int exitDaemonize(const char *pidfile)
{
    return ::unlink(pidfile);
}

static int initDaemon(int nochdir/* = 1*/, int noclose/* = 0*/)
{
#ifdef USE_DAEMON_SYSCALL
    return ::daemon(nochdir, noclose);
#else
    int pid;
    if(pid = ::fork()) // father process
        exit(0);
    else if(pid < 0) // fork error
        return -1;

    if (setsid() != 0) // 脱离控制终端，登录会话和进程组
        return -1;

    if(pid = ::fork())
        exit(0);
    else if(pid < 0)
        return -1;

    if(noclose == 0)
    {
        for(int i = 0; i < NOFILE; ++i)
            ::close(i);
    }

    // 进程活动时，其工作目录所在的文件系统不能卸下。一般需要将工作目录改变到根目录。
    // 对于需要转储核心，写运行日志的进程将工作目录改变到特定目录如/tmp    
    if(nochdir == 0)
    {
        if(::chdir("/") != 0)
            return -1;
    }

    // 将文件创建掩模清除:进程从创建它的父进程那里继承了文件创建掩模,它可能修改守护进程所创建的文件的存取位
    ::umask(0);
    return 0;
#endif
}

static int checkPid(const char *pidfile)
{
    if(pidfile == NULL)
        return -1;

    FILE *f = fopen(pidfile,"r");
    if (f == NULL)
        return -1;

    int pid = 0;
    int n = fscanf(f,"%d", &pid);
    fclose(f);

    if (n !=1 || pid == 0 || pid == ::getpid())
        return -1;

    if (kill(pid, 0) && errno == ESRCH)
        return -1;

    return pid;
}

static int writePid(const char *pidfile)
{
    if(pidfile == NULL)
        return -1;

    int fd = ::open(pidfile, O_RDWR|O_CREAT, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "Now create %s failed.\n", pidfile);
        return -1;
    }

    FILE *file = ::fdopen(fd, "r+");
    if (file == NULL)
    {
        fprintf(stderr, "Now open %s failed.\n", pidfile);
        return -1;
    }

    int pid = 0;
    if (::flock(fd, LOCK_EX|LOCK_NB) == -1)
    {
        int n = fscanf(file, "%d", &pid);
        fclose(file);
        if (n != 1) 
        {
            fprintf(stderr, "Now lock and read pidfile failed.\n");
        } 
        else
        {
            fprintf(stderr, "Now lock pidfile failed, lock is held by pid %d.\n", pid);
        }
        return -1;
    }

    pid = ::getpid();
    if (!fprintf(file,"%d\n", pid)) 
    {
        fprintf(stderr, "Now write pid %d to %s failed.\n", pid, pidfile);
        ::close(fd);
        return -1;
    }
    fflush(file);

    return pid;
}

}
}
