// ***********************************************************************                                                                                                                                                                                                
// Filename         : Process.h
// Author           : LIZHENG
// Description      : 一个通用的、固定数目的watcher-woker多进程模型(亦称prefork模型)
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************#
#ifndef ZL_PROCESS_H
#define ZL_PROCESS_H
#include <unistd.h>
#include <set>
#include <sys/wait.h>
#include <sys/types.h>

// 一个通用的watcher-woker多进程模型，亦称 prefork模型
// 
// see : https://github.com/lighttpd/lighttpd1.4/blob/master/src/server.c
// 主进程创建固定数目的子进程用于进行任务处理，创建完成后主进程阻塞等待子进程退出
// 如果子进程完成任务则退出，此时主进程会再次创建子进程，以使子进程数目维持固定
// 
// while(主进程 and 程序继续运行)
// {
//     if(还有未创建的子进程)
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
//
// if(子进程)  //子进程入口点，用于处理任务
// {
//     // do some work
// }
// else       //主进程退出点，此处杀掉全部子进程，清理资源
// {
//     // killall children process and cleanup
// }

namespace zl
{
    class Process;
    //typedef std::function<void()> ProcessCallback;
    typedef void(*ProcessCallback)(Process *mainProcess, int jodid, void *arg);


    class Process
    {
    public:
        Process();
        ~Process();

        static pid_t mainProcessPid() { return mainProcessPid_; }

    public:
        void createWorkProcess(int workProcessNum, const ProcessCallback& callback, void* arg);

        bool isMainProcess() const { return mainProcessPid() == ::getpid(); }

        bool hasChild() const   { return !childenPids_.empty(); }

        pid_t pid() const { return pid_; }

        bool shutdown() const;

        void stop();

    private:
        pid_t createOneProcess();

    private:
        static pid_t                mainProcessPid_;

        volatile bool               running_;
        pid_t                       pid_;
        std::set<pid_t>             childenPids_;
    };
}

#endif  /* ZL_PROCESS_H */
