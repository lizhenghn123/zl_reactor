#include <iostream>
#include <fstream>
#include <assert.h>
#include "zlreactor/thread/Thread.h"
#include "zlreactor/thread/Mutex.h"
#include "zlreactor/thread/Condition.h"
#include "zlreactor/thread/ThreadPool.h"
#include "zlreactor/thread/ThreadLocal.h"
#include "zlreactor/thread/CountDownLatch.h"
using namespace std;
using namespace zl;
using namespace zl::thread;

namespace thread_usage1
{
// Thread function: Thread ID
void ThreadIDs()
{
    cout << "My thread id is " << this_thread::get_id() << "\n";
}
// Thread function: Yield
void ThreadYield()
{
    this_thread::yield();
}
// Thread function: Detach
void ThreadDetach(int a)
{
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << " .....deteched thread ....\n";
}

Mutex gMutex;
int gCount;
Condition gCond(gMutex);
// Thread function: Condition notifier
void ThreadCondition1()
{
    LockGuard<Mutex> lock(gMutex);
    -- gCount;
    gCond.notify_all();
}
// Thread function: Condition waiter
void ThreadCondition2()
{
    cout << "Wating..." << flush;
    LockGuard<Mutex> lock(gMutex);
    while(gCount > 0)
    {
        cout << "." << flush;
        gCond.wait();
    }
    cout << "." << endl;
}

thread_local int gLocalVar;
// Thread function: Thread-local storage
void ThreadTLS()
{
    gLocalVar = 2;
    cout << " My gLocalVar is " << gLocalVar << "." << endl;
}

void test_threadusage()
{
    // Test 1: Show number of CPU cores in the system
    cout << "PART 1: Info" << "\n";
    cout << "Number of processor cores: " << Thread::hardware_concurrency() << "\n";

    // Test 2: thread IDs
    cout << "\n" << "PART 2: Thread IDs" << "\n";
    {
        // Show the main thread ID
        cout << "Main thread id is " << this_thread::get_id() << "\n";

        // Start a bunch of child threads - only run a single thread at a time
        Thread t1(ThreadIDs);
        this_thread::sleep_for(chrono::milliseconds(100));
        t1.join();

        Thread t2(ThreadIDs);
        this_thread::sleep_for(chrono::milliseconds(100));
        t2.join();

        Thread t3(ThreadIDs);
        t3.join();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    // Test 3: yield
    cout << "\n" << "PART 3: Yield (40 + 1 threads)" << "\n";
    {
        // Start a bunch of child threads
        list<Thread *> threadList;
        for (int i = 0; i < 40; ++i)
            threadList.push_back(new Thread(ThreadYield));

        // main thread Yield...
        this_thread::yield();

        // Wait for the threads to finish
        list<Thread *>::iterator it;
        for (it = threadList.begin(); it != threadList.end(); ++it)
        {
            Thread * t = *it;
            t->join();
            delete t;
        }
    }
    // Test 4: sleep
    cout << "\n" << "PART 4: Sleep (10 x 100 ms)" << "\n";
    {
        cout << "Sleeping" << flush;
        for (int i = 0; i < 10; ++i)
        {
            this_thread::sleep_for(chrono::milliseconds(100));
            cout << "." << flush;
        }
        cout << "\n";
    }
    // Test 5: condition variable
    cout << "\n" << "PART 5: Condition variable (40 + 1 threads)" << "\n";
    {
        // Set the global counter to the number of threads to run.
        gCount = 40;

        // Start the waiting thread (it will wait for gCount to reach zero).
        Thread t1(ThreadCondition2);

        // Start a bunch of child threads (these will decrease gCount by 1 when they
        // finish)
        list<Thread *> threadList;
        for (int i = 0; i < 40; ++i)
            threadList.push_back(new Thread(ThreadCondition1));

        // Wait for the waiting thread to finish
        t1.join();

        // Wait for the other threads to finish
        list<Thread *>::iterator it;
        for (it = threadList.begin(); it != threadList.end(); ++it)
        {
            Thread * t = *it;
            t->join();
            delete t;
        }
    }

    // Test 6: thread local storage
    cout << "\n" << "PART 6: Thread local storage" << "\n";
    {
        // Clear the TLS variable (it should keep this value after all threads are finished).
        gLocalVar = 1;
        cout << "Main gLocalVar is " << gLocalVar << "\n";

        // Start a child thread that modifies gLocalVar
        Thread t1(ThreadTLS);
        t1.join();

        // Check if the TLS variable has changed
        if (gLocalVar == 1)
            cout << "Main gLocalID was not changed by the child thread - OK!" << "\n";
        else
            cout << "Main gLocalID was changed by the child thread - FAIL!" << "\n";
    }
}
}
namespace thread_usage2
{
void func()
{
    cout << "func : " << "\n";
}

void func_one(int i)
{
    cout << "func_one : " << i << "\n";
}

void func_two(int i, float j)
{
    cout << "func_two : " << i << "\t" << j << "\n";
}

class  TestT
{
public:
    static void print_hello()
    {
        std::cout << "hello world\n";
    }

    void print_num(int i)
    {
        std::cout << "num :" << num + i << "\n";
    }

    int num ;
};

void exec_testt(void *arg)
{
    TestT *pt = (TestT*) arg;
    TestT::print_hello();
    pt->print_num(6);
    this_thread::sleep_for(chrono::milliseconds(100));
}

void test_threadusage()
{
    {
        Thread t(std::bind(thread_usage1::ThreadDetach, 43), "d");
        t.detach();
        //this_thread::sleep_for(chrono::milliseconds(100));
        cout << " Detached from thread." << "\n";
    }
    {
        Thread t1(func, "f1");
        t1.join();
    }
    {
        Thread t1(std::bind(func_one, 43), "f2");
        t1.join();
    }
    {
        Thread t1(std::bind(func_two, 43, 123.45), "f3");
        t1.join();
    }
    {
        TestT test;
        test.num = 101;
        Thread t1(std::bind(exec_testt, &test));
        t1.join();
    }
}
}

namespace threadpool_usage
{
    void test_threadpoll()
    {
        ThreadPool pool("ThreadPool");
        pool.start(10);

        pool.run(thread_usage1::ThreadIDs);
        pool.run(thread_usage1::ThreadIDs);
        pool.run(std::bind(thread_usage2::func_one, 12345));
        for (int i = 0; i < 15; ++i)
        {
            pool.run(std::bind(thread_usage2::func_two, i, i * 2.0));
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
        CountDownLatch latch(1);
        pool.run(std::bind(&CountDownLatch::countDown, &latch));
        latch.wait(); //�����ص�ʱ�����̳߳����������������Ѿ��ַ��ˣ�δ�����н�������
        assert(pool.size() == 0);
        pool.stop();
    }
}

int main()
{
    cout << "test_threadusage1------------------------------\n";
    thread_usage1::test_threadusage();

    cout << "test_threadusage2------------------------------\n";
    thread_usage2::test_threadusage();

    cout << "test_threadpoll------------------------------\n";
  //  threadpool_usage::test_threadpoll();

    printf("###### GAME OVER ######\n");
    printf("wait all threads exit, please input any thing to exit....\n");
    getchar();
    return 0;
}
