// ***********************************************************************
// Filename         : PipePairFactory.h
// Author           : LIZHENG
// Created          : 2014-12-18
// Description      : pipe、socketpair、eventfd的封装，可用于线程/进程间通信及同步
//                    未充分验证，可能有bug
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-12-24
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_PIPEPAIRFACTORY_H
#define ZL_PIPEPAIRFACTORY_H
#include "net/SocketUtil.h"
#include "base/ZLog.h"
#include "base/Timestamp.h"
#include "thread/Thread.h"
#include <sys/epoll.h>        // for epoll
#include <sys/eventfd.h>      // for eventfd
using zl::base::Timestamp;
NAMESPACE_ZL_NET_START

template <typename Factory>
class FdPairFactory
{
public:
    FdPairFactory()
    {
        epfd_ = fds_[0] = fds_[1] = -1;
        if(factory_.create(fds_) != 0)
        {
            LOG_ERROR("FdPairFactory: create fds failed[%d]", errno);
        }
        SocketUtil::setNonBlocking(fds_[0]); // fds_[0] for read.
        SocketUtil::setNonBlocking(fds_[1]); // fds_[1] for write.
    }

    ~FdPairFactory()
    {
        close(fds_[0]);
        close(fds_[1]);
        close(epfd_);
    }

    int readFd()
    {
        return fds_[0];
    }

    int writeFd()
    {
        return fds_[1];
    }

    void closeRead()
    {
        close(fds_[0]);
        //fds_[0] = fds_[1];
    }

    void closeWrite()
    {
        close(fds_[1]);
        //fds_[1] = fds_[0];
    }

    size_t write(const void *data, size_t len)
    {
        return SocketUtil::write(fds_[1], data, len);
    }

    size_t read(void *buf, size_t size)
    {
        return SocketUtil::read(fds_[0], buf, size);
    }

    size_t notify()
    {
        char c[1] = {'n'};
        return write(c, 1);
    }

    bool wait(int timeoutMs)
    {
        char c[1];
        if(timeoutMs == 0)
        {
            return read(c, 1) == 1;
        }

        lazyInitEpoll();

        struct epoll_event events[1];
        while(true)
        {
            Timestamp now(Timestamp::now());
            int n = epoll_wait(epfd_, events, 1, timeoutMs);
            if(n < 0)
            {
                LOG_ERROR("FdPairFactory: epoll_wait failed [%d, %d, %d].", epfd_, timeoutMs, errno);
                return false;
            }
            else if(n == 0)
            {
                return false;
            }

            if(read(c, 1) == 1)
            {
                return true;
            }
            else if(errno != EAGAIN && errno != EWOULDBLOCK)
            {
                LOG_ERROR("FdPairFactory: epoll read from[%d] failed[%d]", fds_[0], errno);
                return false;
            }

            double eplase_microms = Timestamp::timediff(Timestamp::now(), now);
            timeoutMs -= static_cast<int>(eplase_microms / 1000);
            if(timeoutMs <= 0)
            {
                return false;
            }
        }

        return true;
    }

private:
    void lazyInitEpoll()
    {
        if(epfd_ > 0)
            return ;

        epfd_ = epoll_create(2);
        if(epfd_ == -1)
        {
            LOG_ERROR("FdPairFactory: epoll_create failed[%d]", errno);
        }
        struct epoll_event event;
        memset(&event, 0, sizeof(event));
        event.events = EPOLLIN;
        if(epoll_ctl(epfd_, EPOLL_CTL_ADD, fds_[0], &event) != 0)
        {
            LOG_ERROR("FdPairFactory: epoll_ctl failed. [%d, %d, %s]", epfd_, fds_[0], errno);
        }
    }

    void close(int fd)
    {
        if(fd != -1)
        {
            ::close(fd);
            fd = -1;
        }
    }

private:
    Factory     factory_;
    int         fds_[2];
    int         epfd_;
};

class PipePairGenerator
{
public:
    int create(int fds[2])
    {
        return ::pipe(fds);
    }
};

class SocketPairGenerator
{
public:
    int create(int fds[2])
    {
        return ::socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    }

private:
    int socketpair2(int af, int type, int protocol, int fd[2]) // one implement of socketpair
    {
        int listen_socket;
        struct sockaddr_in sin[2];
        int len;

        if(type != SOCK_STREAM)  //The following is only valid if type == SOCK_STREAM
            return -1;
        /* Create a temporary listen socket; temporary, so any port is good */
        listen_socket = socket(af, type, protocol);
        if(listen_socket < 0)
        {
            perror("creating listen_socket");
            return -1;
        }
        sin[0].sin_family = af;
        sin[0].sin_port = 0; /* Use any port number */
        sin[0].sin_addr.s_addr = INADDR_ANY;
        if(bind(listen_socket, (struct sockaddr *)&sin[0], sizeof(sin[0])) < 0)
        {
            perror("bind");
            return -1;
        }
        len = sizeof(sin[0]);
        /* Read the port number we got, so that our client can connect to it */
        if(getsockname(listen_socket, (struct sockaddr *)&sin[0], (socklen_t *)&len) < 0)
        {
            perror("getsockname");
            return -1;
        }
        /* Put the listen socket in listening mode */
        if(listen(listen_socket, 5) < 0)
        {
            perror("listen");
            return -1;
        }
        /* Create the client socket */
        fd[1] = socket(af, type, protocol);
        if(fd[1] < 0)
        {
            perror("creating client_socket");
            return -1;
        }
        /* Put the client socket in non-blocking connecting mode */
        fcntl(fd[1], F_SETFL, fcntl(fd[1], F_GETFL, 0) | O_NDELAY);
        if(connect(fd[1], (struct sockaddr *)&sin[0], sizeof(sin[0])) < 0)
        {
            perror("connect");
            return -1;
        }
        /* At the listen-side, accept the incoming connection we generated */
        len = sizeof(sin[1]);
        if((fd[0] = accept(listen_socket, (struct sockaddr *)&sin[1], (socklen_t *)&len)) < 0)
        {
            perror("accept");
            return -1;
        }
        /* Reset the client socket to blocking mode */
        fcntl(fd[1], F_SETFL, fcntl(fd[1], F_GETFL, 0) & ~O_NDELAY);
        close(listen_socket);
        return 0;
    }
};

class TcpPairGenerator
{
public:
    int create(int fds[2])
    {
        //return socketpair2(AF_UNIX, SOCK_STREAM, 0, fds);
		int listenSock = SocketUtil::createSocketAndListen("0.0.0.0", 0, 5);
		SocketUtil::setNonBlocking(listenSock);

		int clientSock = SocketUtil::createSocket();
		struct sockaddr_in addr = SocketUtil::getLocalAddr(listenSock);
		SocketUtil::setNonBlocking(clientSock);

		int ret = SocketUtil::connect(clientSock, addr);   // non-block connect
		if(ret != 0 && errno != EINPROGRESS) // EINPROGRESS = 115
		{
			LOG_ERROR("TcpPairGenerator: connect failed[%d][%d][%d].\n", clientSock, ret, errno);
			SocketUtil::closeSocket(listenSock);
			SocketUtil::closeSocket(clientSock);
			return -1;
		}
		
		struct sockaddr_in addr2;
		int srvSock = SocketUtil::acceptOne(listenSock, &addr2);
		if(srvSock < 0 || errno != EINPROGRESS)
		{
			LOG_ERROR("TcpPairGenerator: accept failed[%d][%d].\n", srvSock, errno);
			SocketUtil::closeSocket(listenSock);
			SocketUtil::closeSocket(clientSock);
			return -1;
		}

		SocketUtil::setNonBlocking(srvSock, false);
		SocketUtil::setNonBlocking(clientSock, false);
		fds[0] = srvSock;
		fds[1] = clientSock;

		SocketUtil::closeSocket(listenSock);

		return 0;
    }
};

class EventFdGenerator
{
public:
    int create(int fds[2])
    {
         fds[0] = fds[1] = createEventfd();
         return 0;
    }

private:
    int createEventfd()
    {
        int efd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (efd < 0)
        {
            LOG_ERROR("create eventfd failed when EventLoop::EventLoop");
            assert(efd);
        }
        LOG_INFO("EventFdGenerator::createEventfd [%d]", efd);
        return efd;
    }
};

typedef FdPairFactory< PipePairGenerator >    PipePairFactory;
typedef FdPairFactory< SocketPairGenerator >  SocketPairFactory;
typedef FdPairFactory< TcpPairGenerator >     TcpPairFactory;
typedef FdPairFactory< EventFdGenerator >     EventFdPairFactory;

NAMESPACE_ZL_NET_END
#endif  /* ZL_PIPEPAIRFACTORY_H */
