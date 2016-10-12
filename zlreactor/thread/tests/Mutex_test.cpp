#include <iostream>
#include <vector>
#include "zlreactor/thread/Thread.h"
#include "zlreactor/thread/Mutex.h"
#include "zlreactor/thread/ThreadGroup.h"
#include "zlreactor/base/StopWatch.h"
#include "zlreactor/base/Timestamp.h"
using namespace std;
using namespace zl;
using namespace zl::base;
using namespace zl::thread;

namespace test_DeadLock
{
    class DeadLock
    {
    public:
        void visit()
        {
            LockGuard<Mutex> lock(mutex_);
            cout << "visit this\n";
            // visit and process
            print();   // just for debug, 导致了死锁， 通过gdb调试命令thread apply all bt查看所有线程的调用堆栈
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

namespace bench
{
	const int kMaxCount = 10 * 1000 * 1000;
	const int kMaxThreads = 10;
	Mutex gMutex;
	vector<int> gVector;
	
	void func_noMutex()
	{
		gVector.reserve(kMaxCount);
		zl::base::StopWatch watch;
		for(int i = 0; i < kMaxCount; ++i)
		{
			gVector.push_back(i);
		}
		cout << "elpased " << watch.elapsedTimeInMicro() << " us \t by func_noMutex elpased.\n"; 
		vector<int>().swap(gVector);
	}
	void func_Mutex_singleThread()
	{
		gVector.reserve(kMaxCount);
		zl::base::StopWatch watch;
		for(int i = 0; i < kMaxCount; ++i)
		{
			LockGuard<Mutex> lock(gMutex);
			gVector.push_back(i);
		}
		cout << "elpased " << watch.elapsedTimeInMicro() << " us \t by func_Mutex_singleThread elpased.\n"; 
		vector<int>().swap(gVector);
	}
	void func_Mutex_multiThread()
	{
		zl::base::StopWatch watch;
		for(int i = 0; i < kMaxCount; ++i)
		{
			LockGuard<Mutex> lock(gMutex);
			gVector.push_back(i);
		}
		cout << "elpased " << watch.elapsedTimeInMicro() << " us \t by func_Mutex_multiThread.\n"; 
	}
	
	void bench_Mutex()
	{
		func_noMutex();							// 单线程且无锁版本
		//return;
		
		func_Mutex_singleThread();				// 单线程加锁版本
		
		gVector.reserve(kMaxCount * kMaxThreads);	
		ThreadGroup trd_group;
		zl::base::StopWatch watch;
		for(int i = 0; i < kMaxThreads; i++)	// 多线程版本，不过这个时间统计包括了创建线程的时间
		{
			trd_group.create_thread(std::bind(func_Mutex_multiThread));
		}
		trd_group.join_all();
		cout << "elpased " << watch.elapsedTimeInMicro() << " us \t by total " << kMaxThreads << " threads.\n"; 
	}
}
int main()
{
    //test_DeadLock::test_MutexDeadLock();
	
	bench::bench_Mutex();
    cout << "####### GAME OVER ######\n";
}
