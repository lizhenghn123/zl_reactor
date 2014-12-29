/*************************************************************************
	File Name   : socketAcceptThunderingHerd.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: Mon 29 Dec 2014 10:21:57 PM CST
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>

/**
 * 阻塞方式时，accept好像并不惊群。
 * 比如使用epoll来管理listenfd，如果设置为非阻塞的，可能出现"惊群"现象
 * 
 */
int create_master_worker_process(int child_num, void (*callback)(void *));
void child_process_accept(void *);  // just call ::accept
void child_process_epoll(void *);   // create epoll and accept
int createSocketAndListen(const char *ip, int port, int backlog);
int setNonBlocking(int fd, bool nonBlocking/* = true*/);

#define CHILD_PROCESS_USE_EPOLL

int listenfd;
int epfd;   // not use

int main(int argc, char *argv[])
{
    // create listen socket before creating worker process
    listenfd = createSocketAndListen("127.0.0.1", 8888, 5);
    assert(listenfd > 0);

    // // epoll fd create before creating child process
    //epfd = ::epoll_create(1024);
    //assert(epfd > 0 && " epoll create failure!");
    //struct epoll_event ev;
    //ev.data.fd = listenfd;
    //ev.events = EPOLLIN;
    //epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    // create worker process, and bind callback for accepting client
    #ifdef CHILD_PROCESS_USE_EPOLL
        setNonBlocking(listenfd, true);        
        create_master_worker_process(20, child_process_epoll);
    #else
        create_master_worker_process(20, child_process_accept);
    #endif

    return 0;
}

void child_process_accept(void *arg)
{
    int childId = *((int*)arg);
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    while(1)
    {
        int connfd = ::accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("child[%d] process[%d] : accept [%d], error[%d]\n", childId, ::getpid(), connfd, errno);
        //::close(connfd);
    }  
}

void child_process_epoll(void *arg)
{
    int childId = *((int*)arg);
    int epfd = ::epoll_create(1024);
    assert(epfd > 0 && " epoll create failure!");
    struct epoll_event ev;
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    struct epoll_event events[128];
    while(1)
    {
        int nfds = epoll_wait(epfd, events, 128, -1);
        //printf("child[%d] process[%d] : events[%d]\n", childId, ::getpid(), nfds);
        for(int i=0; i < nfds; ++i)
        {
            if(events[i].data.fd == listenfd)
            {
                struct sockaddr_in cliaddr;
                socklen_t clilen = sizeof(cliaddr);
                int connfd = ::accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
                if(connfd < 0)
                    printf("child[%d] process[%d] : error on epoll accept [%d], error[%d:%s]\n", childId, ::getpid(), connfd, errno, strerror(errno));
                else
                    printf("child[%d] process[%d] : epoll accept [%d], error[%d]\n", childId, ::getpid(), connfd, errno);
            }
        }
    }
}

int create_master_worker_process(int child_num, void (*callback)(void *))
{
    assert(child_num >= 0);
    if(child_num == 0)
    {
        printf("this is father process\n");
        return 0;
    }

    printf("this is master process[%d]\n", ::getpid());

    long childId = 0;
    bool child = false;
    //prefork child
    while(!child)
    {
        if(child_num > 0)    // continue create process
        {
            childId ++;
            switch(fork())
            {
            case -1:         // fork error
                return -1;
            case 0:          // child process, set flag for breaking while-loop
                child = true;
                break;
            default:         // master process
                child_num--;
                break;
            }
        }
        else                 // all children be created, just wait children exit
        {
            int status;
            if(-1 != ::wait(&status))  // one child process exit
            {
                printf("master process: get one child exit\n"); 
                child_num++;
            }
            else
            {
                switch (errno)
                {
                case EINTR:
                    break;
                default:
                    break;
                }
            }
        }
    } // end of while

    //for the parent this is the exit-point
    if (!child)
    {
        return 0;
    }

    // child process do some work
    printf("this is child[%d], process id[%d], father process id[%d]\n", childId, ::getpid(), ::getppid());
    callback(&childId);
}

int createSocketAndListen(const char *ip, int port, int backlog)
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd < 0) return sockfd;

    //setNonBlocking(sockfd, true);  
    struct sockaddr_in  sockaddr;
    ::memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    int nIP = 0;
    if(!ip || '\0' == *ip || 0 == strcmp(ip, "0")
        || 0 == strcmp(ip, "0.0.0.0") || 0 == strcmp(ip, "*"))
    {
        nIP = htonl(INADDR_ANY);
    }
    else
    {
        nIP = inet_addr(ip);
    }
    sockaddr.sin_addr.s_addr = nIP;

    int optval = 1;
    ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));

    int res = ::bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr));
    if(res < 0) return res;

    res = ::listen(sockfd, backlog);
    if(res < 0) return res;

    printf("server socket listening on [%s:%d]\n", ip, port);
    return sockfd;
}

int setNonBlocking(int fd, bool nonBlocking/* = true*/)
{
    int flags = fcntl(fd, F_GETFL);
    if(flags < 0)
        return flags;

    nonBlocking ? flags |= O_NONBLOCK : flags &= (~O_NONBLOCK);
    return fcntl(fd, F_SETFL, flags);
}
