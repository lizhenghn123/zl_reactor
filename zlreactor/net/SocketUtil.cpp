#include "zlreactor/net/SocketUtil.h"
#include <string.h>
#include "zlreactor/base/Logger.h"
NAMESPACE_ZL_NET_START

namespace
{
    class SocketInitialization
    {
    public:
        SocketInitialization()
        {
        #ifdef OS_WINDOWS
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif
        }
        ~SocketInitialization()
        {
        #ifdef OS_WINDOWS
            WSACleanup();
        #endif
        }
    }g_socket_init_once ;
}

ZL_SOCKET SocketUtil::createSocket()
{
    ZL_SOCKET fd = ZL_CREATE_SOCKET(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    //LOG_INFO("createSocket [%d]", fd);
    return fd;
}

int SocketUtil::closeSocket(ZL_SOCKET fd)
{
    if (fd)
    {
        //LOG_INFO("closeSocket [%d]", fd);
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
    ZL_SOCKET sockfd = SocketUtil::createSocket();
    if(sockfd < 0) return sockfd;

    SocketUtil::setReuseAddr(sockfd, true);

    int ret = SocketUtil::bind(sockfd, ip, port);
    if (ret < 0) return ret;

    ret = ZL_LISTEN(sockfd, backlog);
    if (ret < 0) return ret;

    return sockfd;
}

int SocketUtil::bind(ZL_SOCKET sockfd, const char *ip, int port)
{
    ZL_SOCKADDR_IN sockaddr;
    ::memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    //sockaddr_.sin_addr.s_addr = INADDR_ANY;
    int nIP = 0;
    if (!ip || '\0' == *ip || 0 == strcmp(ip, "0") || 0 == strcmp(ip, "0.0.0.0") || 0 == strcmp(ip, "*"))
    {
        nIP = htonl(INADDR_ANY);
    }
    else
    {
        nIP = inet_addr(ip);
    }
    sockaddr.sin_addr.s_addr = nIP;

    return ZL_BIND(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
}

int SocketUtil::bind(ZL_SOCKET sockfd, struct sockaddr_in addr)
{
    return ZL_BIND(sockfd, (struct sockaddr *)&addr, sizeof(addr));
}

int SocketUtil::connect(ZL_SOCKET sockfd, const char *ip, int port)
{
    struct sockaddr_in addr;
    ::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    ::inet_pton(AF_INET, ip, &addr.sin_addr);
    if (errno == EAFNOSUPPORT)
        return errno;

    return SocketUtil::connect(sockfd, addr);
}

int SocketUtil::connect(ZL_SOCKET sockfd, const struct sockaddr_in& addr)
{
    return ZL_CONNECT(sockfd, (sockaddr *)&addr, static_cast<socklen_t>(sizeof(addr)));
}

ZL_SOCKET SocketUtil::accept(ZL_SOCKET sockfd, ZL_SOCKADDR_IN *addr)
{
    ::memset(addr, 0, sizeof(*addr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(*addr));
    ZL_SOCKET connfd = ZL_ACCEPT(sockfd, (sockaddr *)addr, &addrlen);
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

int SocketUtil::setReuseAddr(ZL_SOCKET fd, bool resue /*= true*/)
{
    int optval = resue ? 1 : 0;
    return ZL_SETSOCKOPT(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

int SocketUtil::setReusePort(ZL_SOCKET fd, bool resue /*= true*/)
{
#ifdef SO_REUSEPORT
    int optval = resue ? 1 : 0;
    return ZL_SETSOCKOPT(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
#else
    LOG_WARN("setReusePort failure, see /usr/include/asm-generic/socket.h SO_REUSEPORT need linux kernel >= 3.9");
    return -1;
#endif
}

int SocketUtil::setKeepAlive(ZL_SOCKET fd, bool alive /*= true*/)
{
    int optval = alive ? 1 : 0;
    return ZL_SETSOCKOPT(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

int SocketUtil::setSendTimeout(ZL_SOCKET fd, long long timeoutMs)
{
    struct timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    return ZL_SETSOCKOPT(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

int SocketUtil::getSendTimeout(ZL_SOCKET fd, long long *timeoutMs)
{
    struct timeval tv;
    int ret = ZL_GETSOCKOPT(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    if (ret == 0)
    {
        *timeoutMs = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }
    return ret;
}

int SocketUtil::setRecvTimeout(ZL_SOCKET fd, long long timeoutMs)
{
    struct timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    return ZL_SETSOCKOPT(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

int SocketUtil::getRecvTimeout(ZL_SOCKET fd, long long* timeoutMs)
{
    struct timeval tv;
    if (ZL_GETSOCKOPT(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == 0)
    {
        *timeoutMs = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        return 0;
    }
    return -1;
}

int SocketUtil::setSendBuffer(ZL_SOCKET fd, int readSize)
{
    return ZL_SETSOCKOPT(fd, SOL_SOCKET, SO_SNDBUF, &readSize, sizeof(readSize));
}

int SocketUtil::getSendBuffer(ZL_SOCKET fd, int* readSize)
{
    socklen_t buff_szie = sizeof(socklen_t);
    return ZL_GETSOCKOPT(fd, SOL_SOCKET, SO_SNDBUF, readSize, &buff_szie);
}

int SocketUtil::setRecvBuffer(ZL_SOCKET fd, int writeSize)
{
    return ZL_SETSOCKOPT(fd, SOL_SOCKET, SO_RCVBUF, &writeSize, sizeof(writeSize));
}

int SocketUtil::getRecvBuffer(ZL_SOCKET fd, int* writeSize)
{
    socklen_t buff_szie = sizeof(socklen_t);
    return ZL_GETSOCKOPT(fd, SOL_SOCKET, SO_RCVBUF, writeSize, &buff_szie);
}

int SocketUtil::setOpt(ZL_SOCKET fd, int level, int name, char *value, int len)
{
    assert(value != NULL);
    assert(len > 0);
    return ZL_SETSOCKOPT(fd, level, name, value, static_cast<socklen_t>(len));
}

int  SocketUtil::getOpt(ZL_SOCKET fd, int level, int optname, int& optval)
{
    ZL_SOCKLEN optlen = sizeof(optval);
    return ZL_GETSOCKOPT(fd, level, optname, &optval, &optlen);
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
    ::memset(&localaddr, 0, sizeof(localaddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(localaddr));
    if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
    {
        printf("sockets::getLocalAddr");
    }
    return localaddr;
}

std::string SocketUtil::getLocalIp(ZL_SOCKET sockfd)
{
    struct sockaddr_in localaddr = SocketUtil::getLocalAddr(sockfd);
    char ip[256], tmp[256];
    ZL_SNPRINTF(ip, 256, "%s", ::inet_ntop(AF_INET, &localaddr.sin_addr, tmp, 256));
    return std::string(ip);
}

short SocketUtil::getLocalPort(ZL_SOCKET sockfd)
{
     struct sockaddr_in localaddr = SocketUtil::getLocalAddr(sockfd);
    char buf[16];
    ZL_SNPRINTF(buf, 16, "%d", ntohs(localaddr.sin_port));
    return ::atoi(buf);
}

std::string SocketUtil::getLocalIpPort(ZL_SOCKET sockfd)
{
    struct sockaddr_in localaddr = SocketUtil::getLocalAddr(sockfd);
    char host[256], ip[256];
    ZL_SNPRINTF(host, 256, "%s:%d", ::inet_ntop(AF_INET, &localaddr.sin_addr, ip, 256), ntohs(localaddr.sin_port));
    return std::string(host);
}

struct sockaddr_in SocketUtil::getPeerAddr(ZL_SOCKET sockfd)
{
    struct sockaddr_in peeraddr;
    ::memset(&peeraddr, 0, sizeof(peeraddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
    {
        printf("sockets::getPeerAddr");
    }
    return peeraddr;
}

std::string SocketUtil::getPeerIp(ZL_SOCKET sockfd)
{
    struct sockaddr_in peeraddr = SocketUtil::getPeerAddr(sockfd);
    char ip[256], tmp[256];
    ZL_SNPRINTF(ip, 256, "%s", ::inet_ntop(AF_INET, &peeraddr.sin_addr, tmp, 256));
    return std::string(ip);
}

short SocketUtil::getPeerPort(ZL_SOCKET sockfd)
{
    struct sockaddr_in peeraddr = SocketUtil::getPeerAddr(sockfd);
    char buf[16];
    ZL_SNPRINTF(buf, 16, "%d", ntohs(peeraddr.sin_port));
    return ::atoi(buf);
}

std::string SocketUtil::getPeerIpPort(ZL_SOCKET sockfd)
{
    struct sockaddr_in peeraddr = SocketUtil::getPeerAddr(sockfd);
    char host[256], ip[256];
    ZL_SNPRINTF(host, 256, "%s:%d", ::inet_ntop(AF_INET, &peeraddr.sin_addr, ip, 256), ntohs(peeraddr.sin_port));
    return std::string(host);
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

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen) < 0)
    {
        return errno;
    }
    else
    {
        return optval;
    }
}

NAMESPACE_ZL_NET_END
