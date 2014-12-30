/*************************************************************************
	File Name   : examples/reader_writer/reader_writer.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2014年12月30日 星期二 19时43分35秒
 ************************************************************************/
#include <stdio.h>
#include <vector>
#include "Define.h"
#include "thread/Thread.h"
#include "thread/RWMutex.h"
using namespace std;
using namespace zl::thread;

/***
读者-写者问题
注意： 一般来说读者要多于写者（1个写者），如果没有额外措施保证，写者很难争取到锁
***/
// TODO 需要优化，使写者有优先权得到锁

const static int reader_num = 5;
const static int writer_num = 1;
ZL_STATIC_ASSERT(writer_num == 1);

RWMutex g_rwMutex;

void reader_thread()
{
    while(1)
    {
        if(g_rwMutex.readLock())
        {
            printf("reader thread[%d] : read one\n", this_thread::get_id().pid());
            g_rwMutex.readUnLock();
        }
        //break;
    }
}

void writer_thread()
{
    while(1)
    {
        if(g_rwMutex.writeLock())
        {
            printf("writer thread[%ld] : writer one\n", this_thread::get_id().pid());
            g_rwMutex.writeUnLock();
        }
        //break;
    }
}

int main()
{
    Thread  *writeThreads = new Thread(writer_thread);

    vector<Thread*> readThreads;
    for(int i= 0; i < reader_num; ++i)
    {
        Thread *trd = new Thread(reader_thread);
        readThreads.push_back(trd);
    }

    // join
    writeThreads->join();
    for(int i= 0; i < reader_num; ++i)
        readThreads[i]->join();
}