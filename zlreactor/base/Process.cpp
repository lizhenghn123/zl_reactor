#include "Process.h"
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>         // for strerror
#include <stdlib.h>         // for abort
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/signalfd.h>
#include <sys/syscall.h>

using namespace zl;

/*static*/ volatile sig_atomic_t srv_shutdown = 0;
/*static*/ volatile sig_atomic_t graceful_shutdown = 0;
/*static*/ volatile sig_atomic_t handle_sig_alarm = 1;
/*static*/ volatile sig_atomic_t handle_sig_hup = 0;
/*static*/ volatile sig_atomic_t forwarded_sig_hup = 0;

namespace detail
{
    static void signal_handler(int sig)
    {
        printf("process [%d] get signal no[%d]\n", ::getpid(), sig);
        switch (sig)
        {
        case SIGTERM:			// 15
            srv_shutdown = 1;
            //g_process.setRunningState(0);
            break;
        case SIGINT:			// 2
            if (graceful_shutdown)
            {
                srv_shutdown = 1;
                //g_process.setRunningState(0);
            }
            else
            {
                graceful_shutdown = 1;
            }
            printf("process [%d] SIGINT: [%d][%d]\n", ::getpid(), graceful_shutdown, srv_shutdown);
            break;
        case SIGALRM:			// 14
            handle_sig_alarm = 1;
            break;
        case SIGHUP:			// 1
            handle_sig_hup = 1;
            break;
        case SIGCHLD:
            break;
        }
    }

    static void setSignalHandler()
    {
        signal(SIGPIPE, SIG_IGN);
        signal(SIGUSR1, SIG_IGN);
        signal(SIGALRM, signal_handler);
        signal(SIGTERM, signal_handler);
        signal(SIGHUP , signal_handler);
        signal(SIGCHLD, signal_handler);
        signal(SIGINT , signal_handler);
        signal(SIGALRM, signal_handler);
    }
}

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

pid_t Process::mainProcessPid_ = 0;

Process::Process()
    : running_(0)
    , pid_(::getpid())
{
    detail::setSignalHandler();
}

Process::~Process()
{
    stop();
}

bool Process::shutdown() const
{
    return srv_shutdown;
}

pid_t Process::createOneProcess()
{
    pid_t pid = ::fork();
    if (pid < 0)
    {
        printf("fork error %s", strerror(errno));
        abort();
        return -1;
    }
    else if (pid > 0)      // parent process
    {
        childenPids_.insert(pid);
        return 0;
    }
    else //if (pid == 0)   // child process
    {
        childenPids_.clear();
        //close(m_fd);
        return ::getpid(); // return child process pid
    }
}

void Process::createWorkProcess(int workProcessNum, const ProcessCallback& callback, void* arg)
{
    assert(workProcessNum >= 0);
    assert(mainProcessPid_ <= 0);
    assert(callback);

    running_ = 1;
    mainProcessPid_ = ::getpid();

    if (workProcessNum == 0)
    {
        printf("not create work process,just do work on main process[%d]\n", ::getpid());
        callback(this, 0, arg);
        return ;
    }

    int childId = 0;
    bool child = false;
    while (running_ && !child && !srv_shutdown && !graceful_shutdown)  //prefork child
    {
        if (workProcessNum > 0)    // continue create process
        {
            childId++;
            pid_t pid = ::fork();
            switch (pid)
            {
            case -1:         // fork error
                return;
            case 0:          // child process, set flag for breaking while-loop
                childenPids_.clear();
                child = true;
                break;
            default:         // master process
                childenPids_.insert(pid);
                workProcessNum--;
                break;
            }
        }
        else                 // all children be created, just wait children exit
        {
            int status = 0;
            pid_t pid;
            if ((pid = waitpid(-1, &status, WNOHANG)) > 0)  // one child process exit
            {
                printf("master process: get one child[%d] exit\n", pid);
                workProcessNum++;
                childenPids_.erase(pid);
            }
            else
            {
                switch (errno)
                {
                case EINTR:
                    if (handle_sig_hup)
                    {
                        handle_sig_hup = 0;
                    }
                    break;
                default:
                    break;
                }
            }
        }
    } // end of while

    if (child)             // child process, do some non-trival thing
    {
        printf("child process[%d], jobid[%d]: father process id[%d], current thread[%d]\n", ::getpid(), childId, ::getppid(), gettid());
        callback(this, childId, arg);
        printf("child process[%d], jobid[%d]: exited......\n", ::getpid(), childId);
    }
    else //if (!child)    //for the parent this is the exit-point
    {
        assert(!child);
        printf("master process[%d]: exit, kill all children process\n", mainProcessPid_);
        if (graceful_shutdown) // kill all children
        {
            printf("master process[%d]: graceful_shutdown, kill all child by SIGINT\n", mainProcessPid_);
            for (size_t i = 0; i < childenPids_.size(); ++i)
            {
                kill(0, SIGINT);
            }
        }
        else if (srv_shutdown)
        {
            printf("master process[%d]: srv_shutdown, kill all child by SIGTERM\n", mainProcessPid_);
            for (size_t i = 0; i < childenPids_.size(); ++i)
            {
                kill(0, SIGTERM);
            }
        }

        // do something that need cleanup or close
        // ......
    }
}

void Process::stop()
{
    printf("process[%d] : stopped[%ld]\n", ::getpid(), childenPids_.size());
    if(!running_)
        return;

    running_ = 0;
    for (size_t i = 0; i < childenPids_.size(); ++i)
    {
        kill(0, SIGTERM);
    }
}
