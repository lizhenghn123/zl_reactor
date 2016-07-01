#include <iostream>
#include "Timer.h"
#include "TimerQueue.h"
#include "zlreactor/base/Timestamp.h"
using namespace std;
using namespace zl;
using namespace zl::base;
using namespace zl::net;

void print(Timer *self)
{
    Timestamp now = Timestamp::now();
    printf("[%s][%0x]async timer trigger : print\n", now.toString().c_str(), self);
    printf("hello world\n");
}

void print_num(Timer *self, int num)
{ 
    Timestamp now = Timestamp::now();
    printf("[%s][%0x]async timer trigger : print_num\n", now.toString().c_str(), self);
    printf("hello world[%d]\n", num);

    num ++;
    if(num > 3)
    {
        self->cancel();
    }
    else
    {
        self->expires_from_now(1200);
        self->async_wait(std::bind(print_num, self, num));
    }
}

void test_sync_timer()
{
    cout << "-----------test_sync_timer-----------\n";
    Timestamp now = Timestamp::now();
    printf("%s\n", now.toString().c_str());
    Timer t1(NULL, 3000);
    
    t1.wait();
    now = Timestamp::now();
    printf("%s\n", now.toString().c_str());
}

void test_async_timer()
{
    cout << "-----------test_async_timer-----------\n";
    TimerQueue tqueue ;
    Timer t1(&tqueue, 1000);
    Timer t2(&tqueue, 2300);

    Timestamp now = Timestamp::now();
    printf("[%s]async timer start\n", now.toString().c_str());

    t1.async_wait(std::bind(print, &t1));
    t2.async_wait(std::bind(print_num, &t2, 1));

    zl::thread::this_thread::sleep_for(zl::thread::chrono::seconds(15));
    printf("================\n");
}

int main()
{
    test_sync_timer();
    test_async_timer();

    TimerQueue tqueue ;

    return 0;
}
