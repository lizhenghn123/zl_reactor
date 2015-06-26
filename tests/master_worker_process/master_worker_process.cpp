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
#include <string.h>
#include <set>

class Fork
{
public:
    Fork() : pid_(::fork())
    {
        if (pid_ < 0)
            throw;
    }
    ~Fork()
    {
        if (pid_)
            wait();
    }

    pid_t pid() const     { return pid_; }
    bool parent() const   { return pid_ > 0; }
    bool child() const    { return !parent(); }
    void setNowait()      { pid_ = 0; }
    int wait(int options = 0)
    {
        int status;
        ::waitpid(pid_, &status, options);
        pid_ = 0;
        return status;
    }

private:
    Fork(const Fork&);
    Fork& operator= (const Fork&);

    pid_t pid_;
};

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

//#define TEST_SUB_PROCESS_EXIT

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
    case SIGTERM:			// 15
        srv_shutdown = 1;
        break;
    case SIGINT:			// 2
        if (graceful_shutdown)
            srv_shutdown = 1;
        else
            graceful_shutdown = 1;
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

void child_process_work(void *arg)
{
    int childId = *((int*)arg);
    while (!srv_shutdown)
    {
        printf("childId [%d] : process[%d] just sleep 6s\n", childId, ::getpid());
        ::sleep(6);
#if defined(TEST_SUB_PROCESS_EXIT)
        break;
#endif
    }
    printf("this is child[%d], process id[%d] break the work\n", childId, ::getpid());
}

int create_master_worker_process(int child_num, void(*callback)(void *arg))
{
    assert(child_num >= 0);
    if (child_num == 0)
    {
        printf("this is father process\n");
        return 0;
    }

    printf("this is master process[%d]\n", ::getpid());

    long childId = 0;
    bool child = false;
    std::set<pid_t> children;

    //prefork child
    while (!child && !srv_shutdown && !graceful_shutdown)
    {
        if (child_num > 0)    // continue create process
        {
            childId++;
            pid_t pid = ::fork();
            switch (pid)
            {
            case -1:         // fork error
                printf("fork error [%s]\n", strerror(errno));
                return -1;
            case 0:          // child process, set flag for breaking while-loop
                child = true;
                children.clear();
                break;
            default:         // master process
                child_num--;
                children.insert(pid);
                break;
            }
        }
        else                 // all children be created, just wait children exit
        {
#if 0
            int status;
            if (-1 != ::wait(&status))  // one child process exit
            {
                printf("master process: get one child exit\n");
                child_num++;
            }
#else       // 这种情况下，在取消TEST_SUB_PROCESS_EXIT宏的定义下，经常出现部分子进程无法杀掉的情况， 上面也会出现！！！
            int status = 0;
            pid_t pid;
            if ((pid = waitpid(-1, &status, WNOHANG)) > 0)  // one child process exit
            {
                printf("master process: get one child[%d] exit\n", pid);
                child_num++;
                children.erase(pid);
            }
#endif
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

    if (child)	// child process, do some non-trival thing
    {
        printf("this is child[%d], process id[%d], father process id[%d], current thread id[%d]\n", childId, ::getpid(), ::getppid(), gettid());
        callback(&childId);
        printf("this is child[%d], process id[%d] exit\n", childId, ::getpid());
    }
    else //if (!child)    //for the parent this is the exit-point
    {
        assert(!child);
        printf("master process exit, kill all children process\n");
        if (graceful_shutdown)  // kill all children too
        {
            printf("master process : graceful_shutdown, kill all child by SIGINT\n");
            for (size_t i = 0; i < children.size(); ++i)
            {
                kill(0, SIGINT);
            }
        }
        else if (srv_shutdown)
        {
            printf("master process : srv_shutdown, kill all child by SIGTERM\n");
            for (size_t i = 0; i < children.size(); ++i)
            {
                kill(0, SIGTERM);
            }
        }

        // do something that need cleanup or close
        // ......
        return 0;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("error, should run as : bin child_num, ex : %s 3\n", argv[0]);
        return 0;
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGUSR1, SIG_IGN);
    signal(SIGALRM, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP , signal_handler);
    signal(SIGCHLD, signal_handler);
    signal(SIGINT , signal_handler);
    signal(SIGALRM, signal_handler);

    create_master_worker_process(atoi(argv[1]), child_process_work);

    return 0;
}

//下面处理动作中字母代表的意义如下：
// A : 缺省动作是终止进程
// B : 缺省动作是忽略此信号
// C : 终止进程并进行内核映像转储（core dump）
// D : 停止进程
// E : 信号不能被捕获
// F : 信号不能被忽略
//         信号 值  处理动作 发出信号的原因
//#define SIGHUP 1      A    终端挂起或者控制进程终止
//#define SIGINT 2      A    键盘中断（Ctrl+c）
//#define SIGQUIT 3     C    键盘的退出键按下
//#define SIGILL 4      C    非法指令
//#define SIGTRAP 5     
//#define SIGABRT 6     C    由abort发出的退出指令
//#define SIGIOT 6      C    IO捕获指令，与SIGABRT同义
//#define SIGBUS 7      
//#define SIGFPE 8      C    浮点异常
//#define SIGKILL 9     AEF  kill信号
//#define SIGUSR1 10
//#define SIGSEGV 11    C    无效的内存引用
//#define SIGUSR2 12
//#define SIGPIPE 13    A    管道破裂（往一个已关闭的管道写数据）
//#define SIGALRM 14    A    由alarm发出的信号
//#define SIGTERM 15    A    终止信号
//#define SIGSTKFLT 16
//#define SIGCHLD 17    B    子进程结束信号
//#define SIGCONT 18         进程继续（曾经被停止）
//#define SIGSTOP 19    DEF  终止进程
//#define SIGTSTP 20
//#define SIGTTIN 21
//#define SIGTTOU 22
//#define SIGURG 23
//#define SIGXCPU 24
//#define SIGXFSZ 25
//#define SIGVTALRM 26
//#define SIGPROF 27
//#define SIGWINCH 28
//#define SIGIO 29
//#define SIGPOLL SIGIO
//
//#define SIGPWR 30
//#define SIGSYS 31
//#define SIGUNUSED 31
//
//#define SIGRTMIN 32
//#define SIGRTMAX _NSIG

