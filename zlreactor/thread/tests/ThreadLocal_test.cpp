#include <iostream>
#include "thread/Thread.h"
#include "thread/Mutex.h"
#include "thread/Condition.h"
#include "thread/ThreadPool.h"
#include "thread/ThreadLocal.h"
#include "thread/CountDownLatch.h"
using namespace std;
using namespace zl;
using namespace zl::thread;

// pod 类型的thread_local测试
namespace thread_tls1
{
    thread_local int gLocalVar = 0;

    void ThreadTLS(int op)
    {
        gLocalVar += op;
        cout << "[" << this_thread::tid() <<"]Thread gLocalVar is " << gLocalVar << ".\n";
    }

    void test_threadtls()
    {
        gLocalVar = 1;   // 主线程设置gLocalVar新值
        cout << "[" << this_thread::tid() << "]Main   gLocalVar is " << gLocalVar << ".\n";

        Thread t1(std::bind(ThreadTLS, 99));   // 子线程更新gLocalVar值， 注意输出
        t1.join();   // cout 99, not 99 + 1

        Thread t2(std::bind(ThreadTLS, 9999));  // 另一子线程更新gLocalVar值， 注意输出
        t2.join();   // cout 9999, not 9999+1

        assert(gLocalVar == 1);
    }
}

// class 类型的thread_local测试
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
        void plus(int n)     { num += n; }
        void print() { cout << "print : [" << this << "] " << num << "\n"; }
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

        Thread t2(std::bind(testTLS, 100));
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
    return 0;
}
