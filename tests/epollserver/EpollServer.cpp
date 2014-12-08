#include "EpollServer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

EpollServer::EpollServer(const char *serverIP, int serverPort)
{
    strcpy(srvIP_, serverIP);
    srvPort_ = serverPort;
    epollFd_ = -1;
}

EpollServer::~EpollServer()
{
    close(srvSocket_);
    close(epollFd_);
}

bool EpollServer::initServer()
{
    srvSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if(srvSocket_ <= 0)
    {
        printf("socket : create server socket error!\n");
        return false;
    }
    sockaddr_in listen_addr;
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(srvPort_);
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_addr.sin_addr.s_addr = inet_addr(srvIP_);

    int reuse_on = 1;
    setsockopt(srvSocket_, SOL_SOCKET, SO_REUSEADDR, &reuse_on, sizeof(reuse_on));

    if(bind(srvSocket_, (sockaddr *)&listen_addr, sizeof(listen_addr)) != 0)
    {
        printf("bind error\n");
        return false;
    }
    if(listen(srvSocket_, 256) < 0)
    {
        printf("listen error!\n");
        return false;
    }
    else
    {
        printf("listening......\n");
    }

    epollFd_ = epoll_create(1);
    int option = O_NONBLOCK;
    if(fcntl(epollFd_, F_SETFL, option) < 0)
    {
        printf("fcntl : set epoll fd error!\n");
        return false;
    }

    if(pthread_create(&listenThread_, 0, (void * ( *)(void *))listenThread, this) != 0)
    {
        printf("pthread_create ： create thread error!");
        return false;
    }

    return true;
}

// 监听线程，负责接收客户端连接，加入到epoll集合中
void EpollServer::listenThread(void *args)
{
    EpollServer *this_server = static_cast<EpollServer *>(args);

    sockaddr_in remote_addr;
    int len = sizeof(remote_addr);
    while(true)
    {
        int client_socket = ::accept(this_server->srvSocket_, (sockaddr *) &remote_addr, (socklen_t *)&len);
        if(client_socket < 0)
        {
            printf("ListenThread : server accept remote client socket error\n");
            continue;
        }
        else
        {
            this_server->setNonBlock(client_socket);
            struct epoll_event    ev;
            ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
            ev.data.fd = client_socket;
            epoll_ctl(this_server->epollFd_, EPOLL_CTL_ADD, client_socket, &ev);
        }
    } 
}

void EpollServer::runLoop()
{
    if(!initServer())
    {
        printf("init server error!!!");
        return;
    }

    const static int MAX_EPOLL_EVENTS = 65535;
    const static int MAX_BUFFER_SIZE = 1024;

    while(true)
    {
        struct epoll_event    events[MAX_EPOLL_EVENTS];
        int nfds = epoll_wait(epollFd_, events,  MAX_EPOLL_EVENTS, -1);
        for(int i = 0; i < nfds; ++i)
        {
            int client_socket = events[i].data.fd;
            char buffer[MAX_BUFFER_SIZE] = { 0 };

            if(events[i].events & EPOLLIN)
            {
                int size = ::recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
                if(size <= 0)
                {
                    printf("recv error: received %d\n", size);
                    struct epoll_event ev;
                    ev.data.fd = client_socket;
                    ev.events = 0;
                    epoll_ctl(epollFd_, EPOLL_CTL_DEL, ev.data.fd, &ev);
                }
                else
                {
                    printf("server received data from [%d] : %s\n", client_socket, buffer);
                    struct epoll_event    ev;
                    ev.events = EPOLLOUT | EPOLLERR | EPOLLHUP;
                    ev.data.fd = client_socket;
                    epoll_ctl(epollFd_, EPOLL_CTL_MOD, client_socket, &ev);
                }
            }
            else if(events[i].events & EPOLLOUT)//监听到写事件，发送数据
            {
                char buffer[MAX_BUFFER_SIZE];
                sprintf(buffer, "Hello World %d\n", client_socket);
                int size = ::send(client_socket, buffer, strlen(buffer) + 1, 0);
                if(size <= 0)
                {
                    struct epoll_event ev;
                    ev.data.fd = events[i].data.fd;
                    ev.events = 0;
                    epoll_ctl(epollFd_, EPOLL_CTL_DEL, ev.data.fd, &ev);
                }
                else
                {
                    struct epoll_event    ev;
                    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
                    ev.data.fd = client_socket;
                    epoll_ctl(epollFd_, EPOLL_CTL_MOD, client_socket, &ev);
                }
            }
            else
            {
                printf("epoll error\n");
                epoll_ctl(epollFd_, EPOLL_CTL_DEL, client_socket, &events[i]);
            }
        }
    }
}

void EpollServer::setNonBlock(int sock)
{
    int old_option = fcntl(sock, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(sock, F_SETFL, new_option);
    //return old_option;
}