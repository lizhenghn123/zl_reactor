#include "net/SocketUtil.h"
#include <string.h>
#include "base/ZLog.h"
NAMESPACE_ZL_NET_START

SocketInitialization  g_socket_init_once;

int SocketUtil::socketInitialise()
{
#ifdef OS_WINDOWS
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    return 0;
}

int SocketUtil::socketCleanup()
{
#ifdef OS_WINDOWS
    return WSACleanup();
#endif
    return 0;
}

ZL_SOCKET SocketUtil::createSocket()
{
    ZL_SOCKET fd = ZL_CREATE_SOCKET(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    LOG_INFO("createSocket [%d]", fd);
    return fd;
}

int SocketUtil::closeSocket(ZL_SOCKET fd)
{
    if (fd)
    {
        LOG_INFO("closeSocket [%d]", fd);
        return ZL_CLOSE(fd);
    }
    return -1;
}

void SocketUtil::shutDown(ZL_SOCKET fd)
{ 
    ::shutdown(fd, SHUT_RDWR);
}

void SocketUtil::shutdownWrite(ZL_SOCKET sockfd)
{
    ::shutdown(sockfd, SHUT_WR);
}

ZL_SOCKET SocketUtil::createSocketAndListen(const char *ip, int port, int backlog)
{
    ZL_SOCKET sockfd = ZL_CREATE_SOCKET(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd < 0) return sockfd;
    setNonBlocking(sockfd, true);

    ZL_SOCKADDR_IN  sockaddr;
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

    int res = ZL_BIND(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr));
    if(res < 0) return res;

    res = ZL_LISTEN(sockfd, backlog);
    if(res < 0) return res;

    return sockfd;
}

int  SocketUtil::connect(ZL_SOCKET sockfd, const struct sockaddr_in& addr)
{
    return ZL_CONNECT(sockfd, (sockaddr *)&addr, static_cast<socklen_t>(sizeof(addr)));
}

ZL_SOCKET SocketUtil::acceptOne(ZL_SOCKET sockfd, ZL_SOCKADDR_IN *addr)
{
    int addrlen = sizeof(*addr);
    ZL_SOCKET connfd = ZL_ACCEPT(sockfd, (sockaddr *)addr, (socklen_t *)&addrlen);
    return connfd;
}

ssize_t SocketUtil::read(ZL_SOCKET sockfd, void *buf, size_t count)
{
    return ZL_READ(sockfd, buf, count);
}

ssize_t SocketUtil::write(ZL_SOCKET sockfd, const void *buf, size_t count)
{
    return ZL_WRITE(sockfd, buf, count);
}

int SocketUtil::setNonBlocking(ZL_SOCKET fd, bool nonBlocking/* = true*/)
{
#if defined(OS_LINUX)
    int flags = ::fcntl(fd, F_GETFL);
    if(flags < 0)
        return flags;

    nonBlocking ? flags |= O_NONBLOCK : flags &= (~O_NONBLOCK);
    return ::fcntl(fd, F_SETFL, flags);
#elif defined(OS_WINDOWS)
    unsigned long ul = nonBlocking ? 1 : 0;
    int ret = ::ioctlsocket(fd, FIONBIO, (unsigned long *)&ul);
    return ret;
#endif
}

int SocketUtil::setNoDelay(ZL_SOCKET fd, bool noDelay/* = true*/)
{
#ifdef OS_LINUX
    int optval = noDelay ? 1 : 0;
#elif defined(OS_WINDOWS)
    bool optval = noDelay;
#endif
    return ZL_SETSOCKOPT(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval));
}

int SocketUtil::setReuseAddr(ZL_SOCKET fd, bool flag /*= true*/)
{
    int optval = flag ? 1 : 0;
    return ZL_SETSOCKOPT(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
}

int SocketUtil::setSendBuffer(ZL_SOCKET fd, int readSize)
{
    return ZL_SETSOCKOPT(fd, SOL_SOCKET, SO_SNDBUF, &readSize, sizeof(readSize));
}

int SocketUtil::setRecvBuffer(ZL_SOCKET fd, int writeSize)
{
    return ZL_SETSOCKOPT(fd, SOL_SOCKET, SO_RCVBUF, &writeSize, sizeof(writeSize));
}

int SocketUtil::getSendBuffer(ZL_SOCKET fd) 
{
    socklen_t buff_szie = sizeof(socklen_t);
    int optname = 0;
    int ret = ZL_GETSOCKOPT(fd, SOL_SOCKET, SO_SNDBUF, &optname, &buff_szie);
    ZL_UNUSED(ret);
    assert(ret != -1);
    return optname > 0 ? optname : 0;
}

int SocketUtil::getRecvBuffer(ZL_SOCKET fd)
{
    socklen_t buff_szie = sizeof(socklen_t);
    int optname = 0;
    int ret = ZL_GETSOCKOPT(fd, SOL_SOCKET, SO_RCVBUF, &optname, &buff_szie);
    ZL_UNUSED(ret);
    assert(ret != -1);
    return optname > 0 ? optname : 0;
}

typedef struct sockaddr SA;
const SA* sockaddr_cast(const struct sockaddr_in* addr)
{
    return static_cast<const SA*>((const void*)addr);
}
SA* sockaddr_cast(struct sockaddr_in* addr)
{
    return static_cast<SA*>((void*)addr);
}

struct sockaddr_in SocketUtil::getLocalAddr(ZL_SOCKET sockfd)
{
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
    {
        printf("sockets::getLocalAddr\"");
    }
    return localaddr;
}

struct sockaddr_in SocketUtil::getPeerAddr(ZL_SOCKET sockfd)
{
    struct sockaddr_in peeraddr;
    bzero(&peeraddr, sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
    {
        printf("sockets::getPeerAddr\"");
    }
    return peeraddr;
}

bool SocketUtil::isSelfConnect(ZL_SOCKET sockfd)
{
    struct sockaddr_in localaddr = getLocalAddr(sockfd);
    struct sockaddr_in peeraddr = getPeerAddr(sockfd);
    return localaddr.sin_port == peeraddr.sin_port
        && localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
}

int SocketUtil::getSocketError(ZL_SOCKET sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof(optval));

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
    {
        return errno;
    }
    else
    {
        return optval;
    }
}

NAMESPACE_ZL_NET_END
