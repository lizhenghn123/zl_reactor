/*************************************************************************
	File Name   : tests/Eventfd_test.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年05月03日 星期一 19时01分59秒
 ************************************************************************/
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include "net/Eventfd.h"
using namespace std;
using namespace zl;
using namespace zl::net;

#define handle_error(msg) do { perror(msg); exit(0); } while(0)

//eventfd与pipe类似，可以用来完成两个线程之间事件触发；甚至进程间消息响应
//eventfd优点：
//1. 比pipe少用了一个文件描述符;
//2. eventfd是固定大小缓冲区（8 bytes），而pipe是可变大小的
//eventfd用法总结
//   如果write过快，那么read时会一次性读取之前write数据的总和，也即：
//   在write之后没有read，但是又write新的数据，那么读取的是这两次的8个字节的和
//   即 如果先write 1 2 3，之后才read那么read的值是1+2+3=6。

// child write more times, parent read once
// 如果是./a.out 1 20 3, 父进程会读取 1 + 20 + 3 = 24
// 如果修改EventfdHandler efd(10), 则父进程会读取 10 + 1 + 20 + 3 = 34
void writemore_readonce(int argc, char *argv[])
{
	EventfdHandler efd;
	if(efd.fd() < 0)
		handle_error("eventfd");
		
	pid_t pid = fork();
	if(pid == 0) // child process
	{
		for (int i = 1; i < argc; i++)
		{
			printf("Child writing %s to efd\n", argv[i]);
			uint64_t u = atoll(argv[i]);
			ssize_t s = efd.write(u);
			if (s != sizeof(uint64_t))
				handle_error("write");
		}
		printf("Child write completed, total write [%d] times.\n", argc-1);
		exit(EXIT_SUCCESS);
	}
	else  // parent process
	{
		sleep(2);   // 等待子进程写完， 一次性读取
		uint64_t u;
		ssize_t s = efd.read(&u);
		//ssize_t s = read(efd.fd(), &u, sizeof(uint64_t));
		if (s != sizeof(uint64_t))
			handle_error("read");
		printf("Parent read %llu from efd\n",(unsigned long long)u);
		exit(EXIT_SUCCESS);
	}
}

// child write more times and parent read the same times
void writemore_readmore(int argc, char *argv[])
{
	EventfdHandler efd(5, 0);  // initval = 5, flags = block
	if(efd.fd() < 0)
		handle_error("eventfd");
   
	pid_t pid = fork();
	if(pid == 0) // child process
	{
		for (int i = 1; i < argc; i++)
		{
			printf("Child writing %s to efd\n", argv[i]);
			uint64_t u = atoll(argv[i]);
			ssize_t s = efd.write(u);
			if (s != sizeof(uint64_t))
				handle_error("write");
			sleep(1);
       }
       printf("Child write completed, total write [%d] times.\n", argc-1);
       exit(EXIT_SUCCESS);
    }
	else  // parent process
	{
		while(1)   // 父进程循环读取
		{
			uint64_t u;
			ssize_t s = efd.read(&u);
			if (s != sizeof(uint64_t))
				handle_error("read");
			printf("Parent read %llu from efd\n",(unsigned long long)u);
           //exit(EXIT_SUCCESS);
       }
   }
}

#define TEST_EventFdPairFactory
void* epoll_read_thread(void *dummy)
{
	EventfdHandler *pefd = (EventfdHandler*)dummy;
	
    int epoll_fd = epoll_create(1024);
	assert(epoll_fd > 0);
	
    struct epoll_event read_event;
    read_event.events = EPOLLHUP | EPOLLERR | EPOLLIN;
#ifdef TEST_EventFdPairFactory
    int fd = pefd->fd();
#else
    int fd = g_eventfd;
#endif
    read_event.data.fd = fd;
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &read_event);
    if (ret < 0)
    {
        perror("epoll ctl failed:");
        return NULL;
    }

    // sleep(8);  //等主线程写完
    const static size_t max_events = 10;
    struct epoll_event events[max_events];
    while (1)
    {
        ret = epoll_wait(epoll_fd, events, max_events, 6000);
        if (ret > 0)
        {
            printf("epoll_wait [%d]\n", ret);
            for (int i = 0; i < ret; i++)
            {
                if (events[i].events & EPOLLHUP)
                {
                    printf("epoll eventfd has epoll hup.\n");
                    break;
                }
                else if (events[i].events & EPOLLERR)
                {
                    printf("epoll eventfd has epoll error.\n");
                    break;
                }
                else if (events[i].events & (EPOLLIN | EPOLLPRI))
                {
                    int event_fd = events[i].data.fd;
                    uint64_t count = 0;
                    //size_t n = ::read(event_fd, &count, sizeof(count));
                #ifdef TEST_EventFdPairFactory
                    size_t n = pefd->read(&count);
                #else
                    size_t n = ::read(event_fd, &count, sizeof(count));
                #endif
                    assert(n == sizeof(uint64_t) && "eventfd every read sizeof(uint64_t) data\n");
                    if (n < 0)
                    {
                        perror("read fail1:");
                        break;
                    }
                    else if(n == 0)
                    {
                        perror("read fail2:");
                        break;
                    }
                    else
                    {
                        struct timeval tv;
                        gettimeofday(&tv, NULL);
                        printf("success  read, read %d bytes(%llu) at %lds %ldus\n", n, count, tv.tv_sec, tv.tv_usec);
                    }
                }
                ////下面两个分支取消之后就无法write-read交互了，
                //else if (events[i].events & EPOLLOUT)   
                //{
                //    printf("EPOLLOUT [%d][%d][%d]\n", ret, errno, events[i].events);
                //}
                //else   // occur some errors[8][0][0]  why????
                //{
                //    printf("occur some errors[%d][%d][%d]\n", ret, errno, events[i].events);
                //}
            }
        }
        else if (ret == 0)
        {
            printf("epoll wait timed out.\n");
            continue;
        }
        else
        {
            perror("epoll wait error:");
            break;
        }
    }

    if (epoll_fd >= 0)
    {
        close(epoll_fd);
        epoll_fd = -1;
    }

    return NULL;
}

void eventfd_epoll(int argc, char *argv[])
{
	EventfdHandler efd;
	
	pthread_t pid = 0;
	pthread_create(&pid, NULL, epoll_read_thread, &efd);
	
	for (int i = 1; i < argc; i++)
	{
		sleep(1);   //如果取消，那么这里很快write完成，epoll线程只能读到一次数据
		printf("Child writing %s to efd\n", argv[i]);
		uint64_t u = atoll(argv[i]);
		ssize_t s = efd.write(u);
		if (s != sizeof(uint64_t))
			handle_error("write");
		else
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            printf("success write, write %d bytes(%llu) at %lds %ldus\n", s, i, tv.tv_sec, tv.tv_usec);
        }
	}
	printf("send data over\n");
    pthread_join(pid, NULL);
}

int main(int argc, char *argv[])
{
	//writemore_readonce(argc, argv);
	//writemore_readmore(argc, argv);
	eventfd_epoll(argc, argv);
    return 0;
}
