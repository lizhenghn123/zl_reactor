#include "EchoClient.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "EpollPoller.h"

const char *SEND_MSG = "hello world!";

EchoClient::EchoClient(const char *serverIP, int serverPort, int connNum)
{
    strcpy(srvIP_, serverIP);
    srvPort_ = serverPort;
    maxConnNum_ = connNum;
    running_ = false;
    epoller_ = new EpollPoller;
}

EchoClient::~EchoClient()
{
    delete epoller_;
}

bool EchoClient::start()
{
    createSockets();
    running_ = true;
}

void EchoClient::createSockets()
{
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, srvIP_, &address.sin_addr);
    address.sin_port = htons(srvPort_);

    for (int i = 0; i < maxConnNum_; ++i)
    {
        int sockfd = socket(PF_INET, SOCK_STREAM, 0);
        printf("create 1 sock\n");
        if (sockfd < 0)
        {
            printf("create socket fail[%d][%d][%s]\n", i, errno, strerror(errno));
            continue;
        }

        int ret = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
        if (ret == 0)
        {
            printf("build connection %d\n", i);
            setNonBlock(sockfd);
            addSock(sockfd);
        }
        else
        {
            printf("connect server error %d, %d, %d, %s\n", i, ret, errno, strerror(errno));
        }
        usleep(1 * 1000); //1ms
    }
}

bool EchoClient::runLoop()
{
    //    while(1);
    const static int MAX_BUFFER_SIZE = 2048;
    char buffer[MAX_BUFFER_SIZE];
    while (running_)
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

        const epoll_event* events = epoller_->events();
        for (int i = 0; i < nfds; ++i)
        {
            int sockfd = events[i].data.fd;
            if (events[i].events & EPOLLIN)
            {
                if (!sockRead(sockfd, buffer, MAX_BUFFER_SIZE))
                {
                    printf("socket read fail [%d][%d][%s]", sockfd, errno, strerror(errno));
                    delSock(sockfd);
                }
                epoller_->mod(sockfd, true, false);
            }
            else if (events[i].events & EPOLLOUT)
            {
                if (!sockWrite(sockfd, SEND_MSG, (int)strlen(SEND_MSG)))
                {
                    printf("socket write fail [%d][%d][%s]", sockfd, errno, strerror(errno));
                    delSock(sockfd);
                }
                epoller_->mod(sockfd, false, true);
            }
            else if (events[i].events & EPOLLERR)
            {
                printf("events[i].events & EPOLLERR [%d][%d][%s]", sockfd, errno, strerror(errno));
                delSock(sockfd);
            }
        }
    }

    return true;
}

void EchoClient::setNonBlock(int sock)
{
    int old_option = fcntl(sock, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(sock, F_SETFL, new_option);
}

void EchoClient::addSock(int sock)
{
    epoller_->add(sock);
    epoller_->mod(sock, true, false);
}

void EchoClient::delSock(int sock)
{
    epoller_->del(sock);
    ::close(sock);
}

bool EchoClient::sockWrite(int sockfd, const char *buffer, int len)
{
    int bytes_write = 0;
    printf("write out %d bytes to socket %d\n", len, sockfd);
    while (1)
    {
        bytes_write = send(sockfd, buffer, len, 0);
        if (bytes_write == -1)
        {
            return false;
        }
        else if (bytes_write == 0)
        {
            return false;
        }

        len -= bytes_write;
        buffer = buffer + bytes_write;
        if (len <= 0)
        {
            return true;
        }
    }
}

bool EchoClient::sockRead(int sockfd, char *buffer, int len)
{
    memset(buffer, '\0', len);
    int bytes_read = recv(sockfd, buffer, len, 0);
    if (bytes_read == -1)
    {
        return false;
    }
    else if (bytes_read == 0)
    {
        return false;
    }
    printf("read in %d bytes from socket %d with content: %s\n", bytes_read, sockfd, buffer);

    return true;
}
