/*************************************************************************
	File Name   : Mutex_test.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年06月10日 星期三 20时05分14秒
 ************************************************************************/
#include <iostream>
#include <vector>
#include "thread/Thread.h"
#include "thread/Mutex.h"
#include "thread/CountDownLatch.h"
using namespace std;
using namespace zl;
using namespace zl::thread;

namespace md
{
    class DeadLock
    {
    public:
        void visit()
        {
            LockGuard<Mutex> lock(mutex_);
            cout << "visit this\n";
            // visit and process
            print();   // just for debug, 但是导致了死锁， 通过gdb调试命令thread apply all bt查看所有线程的调用堆栈
        }
        void print() const
        {
            LockGuard<Mutex> lock(mutex_);
            cout << "just print this\n";
        }
    private:
        // some critial 
        mutable Mutex mutex_;
    };
    void test_MutexDeadLock()
    {
        DeadLock dl;
        dl.visit();
    }
}

int main()
{
    md::test_MutexDeadLock();
    cout << "####### GAME OVER ######\n";
}
