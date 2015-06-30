#include <iostream>
#include <vector>
#include "Define.h"
#ifdef OS_WINDOWS
#include <Windows.h>
#else
#include <syscall.h>
#endif
#include "thread/Thread.h"
using namespace std;
using namespace zl;
using namespace zl::thread;

// this_thread::tid() like this
int gettid()
{
#if defined(OS_WINDOWS)
    return static_cast<int>(::GetCurrentThreadId());
#else
    return static_cast<int>(::syscall(SYS_gettid));
#endif
}

// 说明：
// this_thread::get_id() 返回的id可以认为是用户态thread id，在linux下就是pthread_self(), Windows下是GetCurrentThreadId();
// this_thread::tid() 返回的可以认为是内核态的thread id，在linux下是::syscall(SYS_gettid), Windows下仍是GetCurrentThreadId();
// 在linux下，如果一个程序创建多个thread，并进行销毁，那么this_thread::get_id()会有重复，而this_thread::tid()不会。

namespace threadid_test
{
    void ThreadIDs()
    {
        cout << "Thread id=" << this_thread::get_id() << ", tid=" << this_thread::tid() << ", tid=" << gettid() << "\n";
        //cout << pthread_self() << "\n";  // this_thread::get_id() == pthread_self()
    }

    void ThreadDetach(int a)
    {
        this_thread::sleep_for(chrono::milliseconds(2000));
        cout << "Thread id=" << this_thread::get_id() << ", tid=" << this_thread::tid() << ", tid=" << gettid() << "\n";
        //cout << pthread_self() << "\n";
        cout << " .....deteched thread ....\n";
    }

    void test_four()
    {
        vector<Thread*> trds;
        for (int i = 0; i < 4; ++i)
        {
            trds.push_back(new Thread(ThreadIDs));
        }
        for (int i = 0; i < 4; ++i)
        {
            trds[i]->join();
            delete trds[i];
        }
    }
    void test_threadid()
    {
        {
            Thread t(std::bind(ThreadIDs));
            t.join();
        }
        cout << "----------------------\n";

        {
            Thread t(std::bind(ThreadDetach, 43), "d");
            t.detach();
            cout << " Detached from thread." << "\n";
        }
        cout << "----------------------\n";

        {
            test_four();
            cout << "#####\n";
            test_four();
        }
    }
}

int main()
{
    this_thread::sleep(1000);
    //this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "main   id=" << this_thread::get_id() << ", tid=" << this_thread::tid() << ", tid=" << gettid() << "\n";

    threadid_test::test_threadid();

    this_thread::sleep_for(chrono::milliseconds(3000));

    cout << "main   id=" << this_thread::get_id() << ", tid=" << this_thread::tid() << ", tid=" << gettid() << "\n";
    printf("###### GAME OVER ######\n");
    getchar();
    return 0;
}
