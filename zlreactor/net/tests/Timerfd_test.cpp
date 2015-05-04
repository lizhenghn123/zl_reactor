#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "net/EventLoop.h"
#include "net/Timerfd.h"
#include "base/Timestamp.h"
using namespace std;
using namespace zl;
using namespace zl::base;
using namespace zl::net;
/******
#include <sys/timerfd.h>

int timerfd_create(int clockid, int flags);
    用来创建一个定时器描述符timerfd
    第一个参数：clockid指定时间类型，有两个值：
    CLOCK_REALTIME :Systemwide realtime clock. 系统范围内的实时时钟
    CLOCK_MONOTONIC:以固定的速率运行，从不进行调整和复位 ,它不受任何系统time-of-day时钟修改的影响
    第二个参数：flags可以是0或者O_CLOEXEC/O_NONBLOCK。
    返回值：timerfd（文件描述符）

int timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value);
    启动或关闭有fd指定的定时器
    参数：
    fd：timerfd，有timerfd_create函数返回
    fnew_value:指定新的超时时间，设定new_value.it_value非零则启动定时器，否则关闭定时器，如果new_value.it_interval为0，则定时器只定时一次，即初始那次，否则之后每隔设定时间超时一次
    old_value：不为null，则返回定时器这次设置之前的超时时间
    flags：1代表设置的是绝对时间；为0代表相对时间。
    
int timerfd_gettime(int fd, struct itimerspec *curr_value);
    用于获得定时器距离下次超时还剩下的时间。如果调用时定时器已经到期，
    并且该定时器处于循环模式（设置超时时间时struct itimerspec::it_interval不为0），
    那么调用此函数之后定时器重新开始计时。

read(2) 
    read读取timefd超时事件通知
    当定时器超时，read读事件发生即可读，返回超时次数（从上次调用timerfd_settime()启动开始或上次read成功读取开始），
    它是一个8字节的unit64_t类型整数，如果定时器没有发生超时事件，则read将阻塞若timerfd为阻塞模式，
    否则返回EAGAIN 错误（O_NONBLOCK模式），如果read时提供的缓冲区小于8字节将以EINVAL错误返回。
******/
#define handle_error(msg) do { perror(msg); exit(0);  } while(0)

void printTime()  
{    
    struct timeval tv;    
    gettimeofday(&tv, NULL);    
    printf("current time:%ld.%ld ", tv.tv_sec, tv.tv_usec);  
}  

void test_timerfd()
{
    printTime();  
    printf("test_timerfd start\n"); 	

    TimerfdHandler tfd(CLOCK_MONOTONIC, 0);
	Timestamp exp(Timestamp::now() + 3);
	tfd.resetTimerfd(exp, 1000000);  // 在第3s后触发定时器，以后每隔1s触发一次
	
	int total = 0;
    int count = 0;
	while(count++ < 10)
	{
        uint64_t many;  
        ssize_t s = tfd.read(&many);  
        if (s != sizeof(uint64_t))  
            handle_error("read");  
        
        total += many;  
        printTime();  
        printf("read: %llu; total=%llu\n", many, total); 	

        if(count > 5)
        {
            tfd.stop();
            printf("stop the timerfd\n");
        }   
	}
}

int main()
{
	test_timerfd();
    printf("###### GAME OVER ######");
}

