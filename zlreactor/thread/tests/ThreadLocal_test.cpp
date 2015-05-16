/*************************************************************************
	File Name   : ThreadLocal_test.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: Sun 10 May 2015 03:15:05 PM CST
 ************************************************************************/
#include<iostream>
#include "thread/Thread.h"
#include "thread/Mutex.h"
#include "thread/Condition.h"
#include "thread/ThreadPool.h"
#include "thread/ThreadLocal.h"
#include "thread/CountDownLatch.h"
using namespace std;
using namespace zl;
using namespace zl::thread;

#if !defined(_TTHREAD_CPP11_) && !defined(thread_local)
#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__SUNPRO_CC) || defined(__IBMCPP__)
#define thread_local __thread
#else
#define thread_local __declspec(thread)
#endif
#endif

namespace thread_tls1
{

thread_local int gLocalVar;
// Thread function: Thread-local storage
void ThreadTLS()
{
    gLocalVar = 2;
    cout << "My gLocalVar is " << gLocalVar << ".\n";
}

void test_threadtls()
{
    // Clear the TLS variable (it should keep this value after all threads are finished).
    gLocalVar = 1;
    cout << "Main gLocalVar is " << gLocalVar << ".\n";

    // Start child thread that modifies gLocalVar
    Thread t1(ThreadTLS);
    t1.join();

    Thread t2(ThreadTLS);
    t2.join();

    // Check if the TLS variable has changed
    if (gLocalVar == 1)
        cout << "Main gLocalID was not changed by the child thread - OK!" << "\n";
    else
        cout << "Main gLocalID was changed by the child thread - FAIL!" << "\n";
}
}

namespace thread_tls2
{
    class TestTLS
    {
    public:
        TestTLS() 
        {
            num = -1; 
            cout << "TestTLS : [" << this << "] " << num << "\n"; 
        }
        ~TestTLS()
        {
            cout << "~TestTLS : [" << this << "] " << num << "\n"; 
        }
        void set(int n)      { num = n; }
        void plus(int n)     { num +=n; }
        void print() { cout << "plus : [" << this << "] " << num << "\n"; }
    private:
        int num;
    };

    ThreadLocal<TestTLS> g_tls;

    void testTLS(int i)
    {
        g_tls->plus(i);
        g_tls->print();
    }
    void test_threadtls()
    {
        g_tls->set(10);
        g_tls->print();

        Thread t1(std::bind(testTLS, 23));
        t1.join();

        Thread t2(std::bind(testTLS, 12));
        t2.join();

        g_tls->print();
    }
}

void test()
{
    cout << "#######################################\n";
    thread_tls1::test_threadtls();
    cout << "#######################################\n";
    thread_tls2::test_threadtls();
}
int main()
{
    cout << "###### test thread local ######\n";
    test();    
    
    cout << "###### GAME OVER ######\n";
    cout << "please input any char for exiting.\n";
    getchar();
    return 0;
}
