#include "EchoServer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "EpollPoller.h"

//#define USE_THREAD_ON_ACCEPT    // 是否使accept处于单一线程中

EchoServer::EchoServer(const char *serverIP, int serverPort)
{
    strcpy(srvIP_, serverIP);
    srvPort_ = serverPort;
    epoller_ = new EpollPoller;
}

EchoServer::~EchoServer()
{
    close(srvSocket_);
    delete epoller_;
}

bool EchoServer::initServer()
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
        printf("listening[%d]......\n", srvSocket_);
    }

#ifdef USE_THREAD_ON_ACCEPT
    if (pthread_create(&listenThread_, 0, (void * (*)(void *))listenThread, this) != 0)
    {
        printf("pthread_create ： create thread error!");
        return false;
    }
#else
    setNonBlock(srvSocket_);
    epoller_->add(srvSocket_);
#endif

    return true;
}

// 监听线程，负责接收客户端连接，加入到epoll集合中
void EchoServer::listenThread(void *args)
{
    EchoServer *this_server = static_cast<EchoServer *>(args);

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
            this_server->epoller_->add(client_socket);
        }
    } 
}

void EchoServer::runLoop()
{
    if(!initServer())
    {
        printf("init server error!!!");
        return;
    }

    const static int MAX_BUFFER_SIZE = 1024;
    while (true)
    {
        int nfds = epoller_->poll(-1);
        if (nfds < 0)
        {
            printf("===== epoll_wait error[%d][%d]\n", nfds, errno);
            break;
        }
        else if (nfds == 0)
        {
            printf("timeout\n");
        }

        for (int i = 0; i < nfds; ++i)
        {
            struct epoll_event event = epoller_->nextEvent(i);
            int fd = event.data.fd;
            char buffer[MAX_BUFFER_SIZE] = { 0 };
            if (fd == srvSocket_)
            {
                sockaddr_in remote_addr;
                int len = sizeof(remote_addr);
                int MAX_ACCEPT_PER_POLL = 100;
                while (MAX_ACCEPT_PER_POLL-- > 0)   // 每次最多接收100个连接，避免全部处理accept时影响read、write处理
                {
                    sockaddr_in remote_addr;
                    int len = sizeof(remote_addr);
                    int client_socket = ::accept(srvSocket_, (sockaddr *)&remote_addr, (socklen_t *)&len);
                    printf("accept one [%d]\n", client_socket);
                    if (client_socket < 0)
                    {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                        {
                            //printf("$$$$ %d %s\n", errno, strerror(errno));
                        }
                        else   // error
                        {
                            perror("accept");
                        }
                        break;
                    }
                    else
                    {
                        setNonBlock(client_socket);
                        epoller_->add(client_socket);
                    }
                }
            }
            else if (event.events & EPOLLIN)
            {
                int size = ::recv(fd, buffer, MAX_BUFFER_SIZE, 0);
                if (size <= 0)
                {
                    printf("recv error: received %d\n", size);
                    epoller_->del(fd);
                    ::close(fd);
                }
                else
                {
                    printf("server received data from [%d] : %s\n", fd, buffer);
                    epoller_->mod(fd, true, false);  // write : true, read : false
                }
            }
            else if (event.events & EPOLLOUT)//监听到写事件，发送数据
            {
                char buffer[MAX_BUFFER_SIZE];
                sprintf(buffer, "Hello World %d\n", fd);
                int size = ::send(fd, buffer, strlen(buffer) + 1, 0);
                if (size <= 0)
                {
                    epoller_->del(fd);
                    ::close(fd);
                }
                else
                {
                    epoller_->mod(fd, false, true);  // write : false, read : true
                }
            }
            else
            {
                printf("epoll error\n");
                epoller_->del(fd);
                ::close(fd);
            }
        }
    }
}

void EchoServer::setNonBlock(int sock)
{
    int old_option = fcntl(sock, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(sock, F_SETFL, new_option);
    //return old_option;
}