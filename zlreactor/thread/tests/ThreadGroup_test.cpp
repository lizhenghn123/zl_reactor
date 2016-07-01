/*************************************************************************
	File Name   : ThreadLocal_test.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年06月23日 星期二 23时26分50秒
 ************************************************************************/
#include<iostream>
#include "zlreactor/thread/Thread.h"
#include "zlreactor/thread/Mutex.h"
#include "zlreactor/thread/Condition.h"
#include "zlreactor/thread/ThreadPool.h"
#include "zlreactor/thread/ThreadGroup.h"
using namespace std;
using namespace zl;
using namespace zl::thread;

void printI(int i)
{
    printf("this thread[%ld] print %d\n", this_thread::get_id().value(), i);
    this_thread::sleep_for(chrono::milliseconds(1000));
}
void printString(const char* data)
{
    printf("this thread[%ld] print %s\n", this_thread::get_id().value(), data);
}

void test_ThreadGroup()
{
    ThreadGroup tg;
    Thread *trd;
    for (int i = 1; i < 10; i++)
    {
        if (i%2 == 0)
        {
            trd = tg.create_thread(std::bind(printI, i));
        }
        else if(i%3 == 0)
        {
            tg.create_thread(std::bind(printString, "hello world"));
        }
        else
        {
            tg.add_thread(new Thread(std::bind(printI, i)));
        }
    }

    tg.remove_thread(trd); // 该接口并不能使已加入的线程停止运行（创建时即运行），仅仅使该线程不再由ThreadGroup负责删除

    tg.create_thread(std::bind(printString, "ThreadGroup"));

    tg.join_all();

    delete trd;
}

int main()
{
    cout << "###### test ThreadGroup ######\n";
    test_ThreadGroup();
    
    cout << "###### GAME OVER ######\n";
    return 0;
}
