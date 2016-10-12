#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "zlreactor/thread/Thread.h"
#include "zlreactor/thread/CountDownLatch.h"
#include "zlreactor/base/StringUtil.h"
#include "zlreactor/base/Logger.h"
using namespace std;
using namespace zl;
using namespace zl::thread;
using namespace zl::base;

// 测试1: 多个从线程等待主线程释放信号，然后同时开始工作
// 也即 所有子线程等待主线程发起鸣枪命令 
class KidsThreadsWaitMainThread
{
public:
	KidsThreadsWaitMainThread(int numThreads) : latch_(1)
	{
		for (int i = 0; i < numThreads; ++i)
		{
			string name("thread_");
			name += zl::base::toStr(i);
			threads_.push_back(new Thread(std::bind(&KidsThreadsWaitMainThread::threadFunc, this), name));
		}
		LOG_INFO("KidsThreadsWaitMainThread()");
	}
	void run()
	{
		LOG_INFO("KidsThreadsWaitMainThread run: start release signal");
		latch_.countDown();
		LOG_INFO("KidsThreadsWaitMainThread run: start release signal over");
	}
	void joinAll()
	{
		LOG_INFO("void joinAll() start");
		for_each(threads_.begin(), threads_.end(), std::bind(&Thread::join, std::placeholders::_1));
		LOG_INFO("void joinAll() end");
	}
private:
	void threadFunc()
	{
		LOG_INFO("tid(threadFunc)=%d ready, waiting signal...", this_thread::tid());
		latch_.wait();
		LOG_INFO("tid(threadFunc)=%d run, doing....", this_thread::tid());

		this_thread::sleep_for(chrono::milliseconds(4000));
		//while (true) { }

		LOG_INFO("tid(threadFunc)=%d finish, stopped", this_thread::tid());
	}
private:
	CountDownLatch latch_;
	vector<Thread*> threads_;
};


// 测试2: 多个从线程一创建就开始工作，完成工作后通知主线程， 主线程等待所有任务线程完成工作
// 也即 主线程等待所有子线程初始化完毕或者完成任务
class MainThreadWaitKidsThreads
{
public:
	MainThreadWaitKidsThreads(int numThreads) : latch_(numThreads)
	{
		for (int i = 0; i < numThreads; ++i)
		{
			string name("thread_");
			name += zl::base::toStr(i);
			Thread *trd = new Thread(std::bind(&MainThreadWaitKidsThreads::threadFunc, this), name);
			threads_.push_back(trd);
		}
		LOG_INFO("MainThreadWaitKidsThreads()");
	}
	void wait()
	{
		LOG_INFO("MainThreadWaitKidsThreads wait: wait children starting....");
		latch_.wait();      //此处返回时，所有子线程必然
		LOG_INFO("MainThreadWaitKidsThreads wait: wait children started");
	}
	void joinAll()
	{
		LOG_INFO("void joinAll() start");
		for_each(threads_.begin(), threads_.end(), std::bind(&Thread::join, std::placeholders::_1));
		LOG_INFO("void joinAll() end");
	}
private:
	void threadFunc()
	{
		LOG_INFO("tid(threadFunc)=%d start, doing...", this_thread::tid());

		this_thread::sleep_for(chrono::milliseconds(3000));
		//while (true) { }
		latch_.countDown();   // 工作完成，通知主线程，如果此句写在本函数第一行，表示本线程以启动并通知主线程

		LOG_INFO("tid(threadFunc)=%d finish, stopped", this_thread::tid());
	}
private:
	CountDownLatch latch_;
	vector<Thread*> threads_;
};

int main()
{
	LOG_INFO("-------------------");
	LOG_INFO("tid(main)=%d running....", this_thread::tid());
	KidsThreadsWaitMainThread t1(3);
	this_thread::sleep_for(chrono::milliseconds(3000));
	LOG_INFO("tid(main)=%d create thread ok ...", this_thread::tid());
	t1.run();
	this_thread::sleep_for(chrono::milliseconds(1000));
	t1.joinAll();


	LOG_INFO("-------------------\n");
	LOG_INFO("tid(main)=%d running....", this_thread::tid());
	MainThreadWaitKidsThreads t2(3);
	this_thread::sleep_for(chrono::milliseconds(3000));
	LOG_INFO("tid(main)=%d create thread ok ...", this_thread::tid());
	t2.wait();
	this_thread::sleep_for(chrono::milliseconds(1000));
	t2.joinAll();

	LOG_INFO("game over\n");

	return 0;
}
