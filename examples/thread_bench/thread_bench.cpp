/*************************************************************************
	File Name   : thread_bench.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年05月26日 星期二 20时24分29秒
 ************************************************************************/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "thread/Thread.h"
#include "base/Timestamp.h"
using namespace zl;
using namespace zl::base;
using namespace zl::thread;
using namespace std;

// 事实证明： 线程的创建速度还是远远快于进程的创建速度
// see https://computing.llnl.gov/tutorials/pthreads/#WhyPthreads
// see https://computing.llnl.gov/tutorials/pthreads/fork_vs_thread.txt
void threadFunc()
{
    //printf("this thread : %d\n", this_thread::get_id().tid());
}

void thread_bench(int thread_num)
{
    Timestamp start = Timestamp::now();
    for(int i = 0; i < thread_num; ++i)    
    {
        Thread trd(threadFunc);
        trd.join();  // Thread 的析构有问题
        //trd.detach();
    }    
    Timestamp end = Timestamp::now();
    printf("create %d threads costs %f us\n", thread_num, Timestamp::timediff(end, start));
}

void process_bench(int process_num)
{
    Timestamp start = Timestamp::now();
    for(int i = 0; i < process_num; ++i)    
    {
        pid_t p = ::fork();
        if(p == 0) // child
        {
            exit(0);
        }
        else
        {
            ::waitpid(p, NULL, 0);
        }
    }    
    Timestamp end = Timestamp::now();
    printf("create %d process costs %f us\n", process_num, Timestamp::timediff(end, start));
    
}

int main(int argc, char *argv[])
{
    printf("usage: \n    %s\n    %s <thread num>\n", argv[0], argv[0]);
    
    int max_thread_num = 1000;
    if(argc > 1)
    {
         max_thread_num = atoi(argv[1]);
    }

    printf("###### thread ######\n");
    thread_bench(max_thread_num);

    printf("###### process ######\n");
    //process_bench(max_thread_num);

    printf("###### GAME OVER ######");
}
