#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "thread/Thread.h"
#include "thread/CyclicBarrier.h"
#include "base/StringUtil.h"
using namespace std;
using namespace zl;
using namespace zl::thread;
using namespace zl::base;
// see http://www.cnblogs.com/dolphin0520/p/3920397.html   http://www.cnblogs.com/leesf456/p/5392816.html


// 主线程等待所有子线程处理完数据再继续下去(以及是否设置回调)
template<bool TestCallBack, bool TestCycle>
class MainThreadWaitKidsThreads
{
public:
    MainThreadWaitKidsThreads(int numThreads)
    {
		if(TestCallBack)
			barrier_ = new CyclicBarrier(numThreads, std::bind(&MainThreadWaitKidsThreads::process, this));
		else
			barrier_ = new CyclicBarrier(numThreads, NULL);
        for (int i = 0; i < numThreads; ++i)
        {
            string name("thread_");
            name += zl::base::toStr(i);
            threads_.push_back(new Thread(std::bind(&MainThreadWaitKidsThreads::threadFunc, this), name));
        }
        printf("MainThreadWaitKidsThreads()\n");
    }
	
    void process()
    {
        printf("MainThreadWaitKidsThreads : all child thread finished\n");
    }
	
    void joinAll()
    {
        printf("void joinAll() start\n");
        for_each(threads_.begin(), threads_.end(), std::bind(&Thread::join, std::placeholders::_1));
        printf("void joinAll() end\n");
    }
private:
    void threadFunc()
    {
        printf("tid(threadFunc)=%d ready, and start process..\n", this_thread::tid());
		this_thread::sleep_for(chrono::milliseconds(4000));   // 模拟处理过程1
		printf("tid(threadFunc)=%d finished(step 1), and wait other threads finish..\n", this_thread::tid());
		
        int index = barrier_->wait();  // 等待所有子线程处理完数据
		printf("tid(threadFunc)=%d wait over(step 1)  (%d)\n", this_thread::tid(), index);

				
		if(TestCycle)
		{
			this_thread::sleep_for(chrono::milliseconds(4000));   // 模拟处理过程2
			printf("tid(threadFunc)=%d finished(step 2), and wait other threads finish..\n", this_thread::tid());
		
			int index = barrier_->wait();  // 等待所有子线程处理完数据
			printf("tid(threadFunc)=%d wait over(step 2)  (%d)\n", this_thread::tid(), index);
		}
        printf("tid(threadFunc)=%d stop, and terminat..\n", this_thread::tid());
    }
private:
    CyclicBarrier* barrier_;
	bool  testCycle_;
    vector<Thread*> threads_;
};

template <bool TestCallBack, bool TestCycle>
void test_CyclicBarrier()
{
    printf("----TestCallBack = %d, TestCycle = %d ---------------\n", TestCallBack, TestCycle);
    printf("tid(main)=%d running....\n", this_thread::tid());
    MainThreadWaitKidsThreads<TestCallBack, TestCycle> t1(3);
    //this_thread::sleep_for(chrono::milliseconds(3000));
    printf("tid(main)=%d create thread ok ...\n", this_thread::tid());
    this_thread::sleep_for(chrono::milliseconds(1000));
    t1.joinAll();
	printf("######\n\n");
}

int main()
{
	test_CyclicBarrier<false, false>();

	test_CyclicBarrier<true, false>();

	test_CyclicBarrier<false, true>();

	test_CyclicBarrier<true, true>();
	
    printf("game over\n");
    return 0;
}