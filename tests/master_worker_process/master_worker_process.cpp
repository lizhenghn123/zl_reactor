/*************************************************************************                                                                                                                                                                                                    
    File Name   : master_worker_process.cpp
    Author      : LIZHENG
    Mail        : lizhenghn@gmail.com
    Created Time: 2014年12月29日 星期一 20时56分12秒
 ************************************************************************/
#include <stdio.h>
#include <unistd.h>      // for fork and sleep
#include <stdlib.h>      // for atoi
#include <sys/wait.h>    // for ::wait
#include <sys/syscall.h> // for syscall 
#include <errno.h>
#include <assert.h>

// 一个通用的watcher-woker多进程模型，亦称 prefork模型
// 
// see : https://github.com/lighttpd/lighttpd1.4/blob/master/src/server.c
// 主进程创建固定数目的子进程用于进行任务处理，创建完成后主进程阻塞等待子进程退出
// 如果子进程完成任务则退出，此时主进程会再次创建子进程，以使子进程数目维持固定
// 
// while(主进程 and 程序继续运行)
// {
//     if 还有未创建的子进程
//     {
//         创建一个新的子进程
//             if 是子进程, 退出循环.
//             if 是父进程, 将未创建的子进程数量减 1
//     }
//     else //所有子进程都创建完成
//     {
//         阻塞等待子进程的退出,
//         一旦有退出，主进程就得到消息, 并将未创建的子进程数量加 1
//     }
// }

#define TEST_SUB_PROCESS_EXIT

static volatile sig_atomic_t srv_shutdown = 0;
static volatile sig_atomic_t graceful_shutdown = 0;
static volatile sig_atomic_t handle_sig_alarm = 1;
static volatile sig_atomic_t handle_sig_hup = 0;
static volatile sig_atomic_t forwarded_sig_hup = 0;

pid_t gettid()
{
      return static_cast<pid_t>(::syscall(SYS_gettid));
}

static void signal_handler(int sig)
{
    printf("process [%d] get signal no[%d]\n", ::getpid(), sig);
    switch (sig)
    {
    case SIGTERM:
        srv_shutdown = 1; break;
    case SIGINT:
        if (graceful_shutdown)
            srv_shutdown = 1;
        else 
            graceful_shutdown = 1;
        break;
    case SIGALRM:
        handle_sig_alarm = 1; break;
    case SIGHUP:
        handle_sig_hup = 1; break;
    case SIGCHLD:
        break;
    }
}

void child_process_work(void *arg)
{
    int childId = *((int*)arg);
    while(!srv_shutdown)
    {
        printf("childId [%d] : process[%d] just sleep 6s\n", childId, ::getpid());
        ::sleep(6);
    #if defined(TEST_SUB_PROCESS_EXIT)
        break;
    #endif
    }
}

int create_master_worker_process(int child_num, void (*callback)(void *arg))
{
    assert(child_num >= 0);
    if(child_num == 0)
    {
        printf("this is father process\n");
        return 0;
    }

    printf("this is master process[%d]\n", ::getpid());

    long childId = 0;
    bool child = false;
    //prefork child
    while(!child && !srv_shutdown && !graceful_shutdown)
    {
        if(child_num > 0)    // continue create process
        {
            childId ++;
            switch(fork())
            {
            case -1:         // fork error
                return -1;
            case 0:          // child process, set flag for breaking while-loop
                child = true;
                break;
            default:         // master process
                child_num--;
                break;
            }
        }
        else                 // all children be created, just wait children exit
        {
            int status;
            if(-1 != ::wait(&status))  // one child process exit
            {
                printf("master process: get one child exit\n"); 
                child_num++;
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

    //for the parent this is the exit-point
    if (!child)
    {
        printf("master process exit, kill all children process\n");
        // kill all children too
        if (graceful_shutdown)
        {
            kill(0, SIGINT);
        }
        else if (srv_shutdown)
        {
            kill(0, SIGTERM);
        }

        // do something that need cleanup or close
        // ......
        return 0;
    }

    // child process do some work
    printf("this is child[%d], process id[%d], father process id[%d], current thread id[%d]\n", childId, ::getpid(), ::getppid(), gettid());
    callback(&childId);
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("error, should run as : bin child_num, ex : %s 3\n", argv[0]);
        return 0;
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGUSR1, SIG_IGN);
    signal(SIGALRM, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP,  signal_handler);
    signal(SIGCHLD, signal_handler);
    signal(SIGINT,  signal_handler);
    signal(SIGALRM, signal_handler);

    create_master_worker_process(atoi(argv[1]), child_process_work);

    return 0;
}
