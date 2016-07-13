#include <iostream>
#include <assert.h>
#include "zlreactor/thread/Event.h"
#include "zlreactor/thread/Thread.h"
using namespace std;

// SIGNAL 表示开始就有信号, AUTORESET表示在wait/try_wait之后，会重置信号
template <bool SIGNAL, bool AUTORESET>
void test_usage()
{
    cout << "test_usage: (" << (SIGNAL ? "true" : "false") << ", " << (AUTORESET ? "true" : "false") << ")\n";
    {
        zl::thread::Event ev(SIGNAL, AUTORESET);
        assert(ev.try_wait() == SIGNAL);
        assert(ev.timed_wait(1000) == (AUTORESET ? false : SIGNAL));
        ev.set();
        assert(ev.try_wait() == true);
        assert(ev.timed_wait(1000) == !AUTORESET);
        assert(ev.timed_wait(1000) == !AUTORESET);
        if(AUTORESET == false)
        {
            assert(ev.wait() == true);
        }
    }

}

void test_usage()
{
    cout << "step 1 \n";
    {
        zl::thread::Event ev(false, true);
        assert(ev.try_wait() == false);
        assert(ev.timed_wait(1000) == false);
        ev.set();
        assert(ev.try_wait() == true);
        assert(ev.timed_wait(1000) == false);
        assert(ev.timed_wait(1000) == false);
    }

    cout << "step 2 \n";
    {
        zl::thread::Event ev(false, false);
        assert(ev.try_wait() == false);
        assert(ev.timed_wait(1000) == false);
        ev.set();
        assert(ev.try_wait() == true);
        assert(ev.timed_wait(1000) == true);
        assert(ev.timed_wait(1000) == true);
    }


    cout << "step 3 \n";
    {
        zl::thread::Event ev(true, true);
        assert(ev.try_wait() == true);
        assert(ev.timed_wait(1000) == false);
        ev.set();
        assert(ev.try_wait() == true);
        assert(ev.timed_wait(1000) == false);
        assert(ev.timed_wait(1000) == false);
    }


    cout << "step 4 \n";
    {
        zl::thread::Event ev(true, false);
        assert(ev.try_wait() == true);
        assert(ev.timed_wait(1000) == true);
        ev.set();
        assert(ev.try_wait() == true);
        assert(ev.timed_wait(1000) == true);
        assert(ev.timed_wait(1000) == true);
        assert(ev.wait() == true);
    }
}

void test_reset()
{
    {
        zl::thread::Event ev;
        assert(ev.try_wait() == false);
        assert(ev.timed_wait(1000) == false);
        ev.set();
        assert(ev.try_wait() == true);
        assert(ev.timed_wait(1000) == false);
        assert(ev.timed_wait(1000) == false);

        ev.reset();
        assert(ev.try_wait() == false);
        assert(ev.timed_wait(1000) == false);
    }
    {
        zl::thread::Event ev(true, false);
        ev.reset();
        assert(ev.try_wait() == false);
        assert(ev.timed_wait(1000) == false);
    }
}

void test_threads()
{
    {
        int value = 0;
        zl::thread::Event ev(false, false);   /// 如果第二个参数为true，则只能有一个线程能够wait返回，其他的继续阻塞等待
        zl::thread::Thread waiter([&ev]() { ev.wait(); } );
        zl::thread::Thread worker([&value, &ev]()
									{
										while(value <= 20)
										{
											value++;
											zl::thread::this_thread::sleep(100);
										}
										ev.set();
									}
                                 );
        waiter.join();
        worker.join();
        ev.wait();
        cout << "now value is " << value << "\n";
    }
    {
        int value = 0;
        zl::thread::Event ev(false, true);   /// 如果第二个参数为true，则只能有一个线程能够wait返回，其他的继续阻塞等待
        zl::thread::Thread waiter([&ev]() { ev.wait(); } );
        zl::thread::Thread worker([&value, &ev]()
									{
										while(value <= 20)
										{
											value++;
											zl::thread::this_thread::sleep(100);
										}
										ev.set();
									}
                                 );
        waiter.join();
        worker.join();

        ev.set();   // 必须调用此句，否则下面的wait会一直阻塞
        ev.wait();

        cout << "now value is " << value << "\n";
    }
}

int main()
{
	cout << "test_usage()" << "\n";
    test_usage();
    test_usage<false, true>();
    test_usage<false, false>();
    test_usage<true, true>();
    test_usage<true, false>();

    cout << "test_reset()" << "\n";
    test_reset();

    cout << "test_threads()" << "\n";
    test_threads();
    return 0;
}
