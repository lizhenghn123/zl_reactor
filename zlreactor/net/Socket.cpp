#include "net/Socket.h"
#include "net/InetAddress.h"
NAMESPACE_ZL_NET_START

const static int MAX_RECV_SIZE = 64;

/*********
recv read write send 函数的返回值说明:
	< 0 ：出错;
	= 0 ：连接关闭;
	> 0 : 接收/发送数据大小
除此之外，非阻塞模式下返回值 < 0 并且 (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)的情况下认为连接是正常的，继续接收/发送
*********/

Socket::Socket(ZL_SOCKET fd) : sockfd_(fd) 
{
    ::memset(&sockaddr_, 0, sizeof(sockaddr_));
}

Socket::Socket(ZL_SOCKET fd, ZL_SOCKADDR_IN sockAddr)
    : sockfd_(fd), sockaddr_(sockAddr)
{

}

Socket::~Socket()
{
}

bool Socket::bind(const char *ip, int port)
{
    if(!isValid())
    {
        return false;
    }

    sockaddr_.sin_family = AF_INET;
    sockaddr_.sin_port = htons(port);
    //sockaddr_.sin_addr.s_addr = INADDR_ANY;
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
    sockaddr_.sin_addr.s_addr = nIP;

    int ret = ::ZL_BIND(sockfd_, (struct sockaddr *) &sockaddr_, sizeof(sockaddr_));
    if(ret == -1)
    {
        return false;
    }
    return true;
}

bool Socket::bind(const InetAddress& addr)
{
    sockaddr_ = addr.getSockAddrInet();
	int ret = ::ZL_BIND(sockfd_, (struct sockaddr *) &sockaddr_, sizeof(sockaddr_));
    if(ret == -1)
    {
        return false;
    }
    return true;
}

bool Socket::listen(int backlog /*= 5*/) const
{
    if(!isValid())
    {
        return false;
    }

    int ret = ::ZL_LISTEN(sockfd_, backlog);
    if(ret == -1)
    {
        return false;
    }
    return true;
}

bool Socket::accept(Socket& new_socket) const
{
    int addr_length = sizeof(new_socket.sockaddr_);
    new_socket.sockfd_ = ::ZL_ACCEPT(sockfd_, (sockaddr *)&new_socket.sockaddr_, (socklen_t *)&addr_length);

    if(new_socket.sockfd_ <= 0)
        return false;
    else
        return true;
}

ZL_SOCKET Socket::accept(InetAddress *peerAddr) const
{
    printf("------%d\n", sockfd_);
    ZL_SOCKADDR_IN addr;
    ::memset(&addr, 0, sizeof(addr));
    int addr_length = sizeof(addr);
    ZL_SOCKET connfd = ::ZL_ACCEPT(sockfd_, (sockaddr *)&addr, (socklen_t *)&addr_length);
    assert(connfd > 0 && "ffff");
    if (connfd > 0)
    {
        peerAddr->setSockAddrInet(addr);
    }
    return connfd;
}

int Socket::send(const std::string& data) const
{
    return send(data.c_str(), data.size());
}

int Socket::send(const char *data, size_t size)const
{
    int len = ::ZL_SEND(sockfd_, data, size, 0);
    //if (len == -1)     // error
    //else if (len == 0) // connection is closed
    //else               // ok
    return len;
}

int Socket::recv(std::string& data) const
{
    char buf[MAX_RECV_SIZE + 1] = { 0 };

    while(true)
    {
        int len = recv(buf, MAX_RECV_SIZE, false);
        if(len == 0)  // 连接已关闭
        {
            return 0;
        }
        else if(len < 0)
        {
            if(SOCKET_ERROR == SOCKET_ERROR_EAGAIN || SOCKET_ERROR == SOCKET_ERROR_INTERUPT)
                continue;
            else if(SOCKET_ERROR == SOCKET_ERROR_WOULDBLOCK)
                break;
            else
                return len; // 发生了错误
            break;
        }
        else
        {
            //buf[len] = '\0';
            data += buf;
            if(len < MAX_RECV_SIZE)   //全部接收完成
                break;
        }
    }
    return data.size();
}

int Socket::recv(char *data, int length, bool complete /*= false */) const
{
    int received = 0;
    if(complete)
    {
        while(received != length)
        {
            int len = ::ZL_RECV(sockfd_, (char *)data + received, length - received, 0);
            if(len == -1)
            {
                printf("status == -1, errno == [%d], in Socket::recv\n", errno);
                return len;
            }
            else if(len == 0)
            {
                return len;
            }
            else
            {
                received += len;
            }
        }
    }
    else
    {
        received = ::ZL_RECV(sockfd_, (char *)data, length, 0);
    }
    return received;
}

int Socket::sendTo(const std::string& data, int flags, InetAddress& sinaddr)const
{
    return sendTo(data.c_str(), data.size(), flags, sinaddr);
}

int Socket::sendTo(const char *data, size_t size, int flags, InetAddress& sinaddr)const
{
    int len = ::ZL_SENDTO(sockfd_, data, size, flags, sinaddr, sinaddr.addressLength());

    return len;
}

int Socket::recvFrom(std::string& data, int flags, InetAddress& sinaddr)const
{
    char buf[MAX_RECV_SIZE + 1] = { 0 };

    while(true)
    {
        int len = recvFrom(buf, MAX_RECV_SIZE, flags, sinaddr);
        if(len == -1 || len == 0)
            break;
        //buf[len] = '\0';
        data += buf;
        if(len < MAX_RECV_SIZE)
            break;
    }
    return data.size();
}

int Socket::recvFrom(char *data, int length, int flags, InetAddress& sinaddr)const
{
    socklen_t slen;
    int len = ::ZL_RECVFROM(sockfd_, data, length, flags, sinaddr, &slen);
    if(slen != sinaddr.addressLength())
        throw SocketException("unknown protocol type(in Socket::RecvFrom)");
    return len;
}

bool Socket::connect(const std::string& host, int port)
{
    if(!isValid())
    {
        return false;
    }

    sockaddr_.sin_family = AF_INET;
    sockaddr_.sin_port = htons(port);

    int status = inet_pton(AF_INET, host.c_str(), &sockaddr_.sin_addr);
    if(errno == EAFNOSUPPORT)
        return false;

    status = ::ZL_CONNECT(sockfd_, (sockaddr *)&sockaddr_, sizeof(sockaddr_));

    return status == 0 ? true : false;
}

void Socket::close()
{
    if(isValid())
    {
        ::ZL_CLOSE(sockfd_);
    }
}

bool Socket::setBlocking()
{
#if defined(OS_WINDOWS)
    unsigned long ul = 0;

    int ret = ioctlsocket(sockfd_, FIONBIO, (unsigned long *)&ul); //设置成阻塞模式

    if(ret == SOCKET_ERROR)
        return false;

#elif defined(OS_LINUX)
    int flags = fcntl(sockfd_, F_GETFL);
    if(flags < 0)
        return false;

    flags &= (~O_NONBLOCK);
    if(fcntl(sockfd_, F_SETFL, flags) != 0)
        return false;
#endif

    return true;
}

bool Socket::setNonBlocking()
{
#if defined(OS_WINDOWS)
    unsigned long ul = 1;

    int ret = ioctlsocket(sockfd_, FIONBIO, (unsigned long *)&ul);  //设置成非阻塞模式

    if(ret == SOCKET_ERROR)
        return false;

#elif defined(OS_LINUX)
    int flags = fcntl(sockfd_, F_GETFL);
    if(flags < 0)
        return false;

    flags |= O_NONBLOCK;
    if(fcntl(sockfd_, F_SETFL, flags) != 0)
        return false;
#endif

    return true;
}

bool Socket::setNoDelay(bool flag /*= true*/)
{
    int optval = flag ? 1 : 0;
    return setOpt(IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval));
}

bool Socket::setReuseAddr(bool flag /*= true*/)
{
    int optval = flag ? 1 : 0;
    return setOpt(SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
}

bool Socket::setKeepAlive(bool flag /*= true*/)
{
    int optval = flag ? 1 : 0;
    return setOpt(SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, sizeof(optval));
}

bool Socket::setSendBuffer(int size)
{
    return ZL_SETSOCKOPT(sockfd_, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) == 0;
}
      
bool Socket::getSendBuffer(int& size)
{
    return (ZL_GETSOCKOPT(sockfd_, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) == 0);
}

bool Socket::setRecvBuffer(int size)
{
    return ZL_SETSOCKOPT(sockfd_, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) == 0;
}

bool Socket::getRecvBuffer(int& size)
{
    return (ZL_GETSOCKOPT(sockfd_, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) == 0);
}

bool Socket::setSendTimeout(int sendTimeoutSec, int sendTimeoutUsec/* = 0*/)
{
    struct timeval time;
    time.tv_sec = sendTimeoutSec;
    time.tv_usec = sendTimeoutUsec;

    return ZL_SETSOCKOPT(sockfd_, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(struct timeval)) == 0;
}

bool Socket::getSendTimeout(int& sendTimeoutSec, int& sendTimeoutUsec)
{
    struct timeval time;
    if(ZL_GETSOCKOPT(sockfd_, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(struct timeval)) == 0)
    {
        sendTimeoutSec = time.tv_sec;
        sendTimeoutUsec = time.tv_usec;
        return true;
    }
    return false;
}

bool Socket::setReceiveTimeout(int recvTimeoutSec, int recvTimeoutUsec/* = 0*/)
{
    struct timeval time;
    time.tv_sec = recvTimeoutSec;
    time.tv_usec = recvTimeoutUsec;

    return ZL_SETSOCKOPT(sockfd_, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(struct timeval)) == 0;
}

bool Socket::getReceiveTimeout(int& recvTimeoutSec, int& recvTimeoutUsec)
{
    struct timeval time;
    if(ZL_GETSOCKOPT(sockfd_, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(struct timeval)) == 0)
    {
        recvTimeoutSec = time.tv_sec;
        recvTimeoutUsec = time.tv_usec;
        return true;
    }
    return false;
}

bool Socket::setLinger(bool enable, int waitTimeSec /*= 5*/)
{
    linger lngr;
    lngr.l_onoff = enable ? 1 : 0;
    lngr.l_linger = waitTimeSec;

    return ZL_SETSOCKOPT(sockfd_, SOL_SOCKET, SO_LINGER, &lngr, sizeof(linger)) == 0;
}

bool Socket::getLinger(bool& enable, int& waitTimeSec)
{
    linger lngr;
    if(ZL_GETSOCKOPT(sockfd_, SOL_SOCKET, SO_RCVTIMEO, &lngr, sizeof(lngr)) == 0)
    {
        enable = lngr.l_onoff;
        waitTimeSec = lngr.l_linger;
        return true;
    }
    return false;
}

bool Socket::setOpt(int level, int name, char *value, int len)
{
    assert(value != NULL);
    assert(len > 0);
    return ZL_SETSOCKOPT(sockfd_, level, name, value, len) == 0;
}

bool  Socket::getOpt(int level, int optname, int& optval)
{
    ZL_SOCKLEN optlen = sizeof(optval);

    if(ZL_GETSOCKOPT(sockfd_, level, optname, &optval, &optlen) < 0)
        return false;
    return true;
}

short Socket::getHostPort()
{
    char buf[16];
    ZL_SNPRINTF(buf, 16, "%d", ntohs(sockaddr_.sin_port));
    return ::atoi(buf);
}

std::string Socket::getHostIP()
{
    char ip[256], tmp[256];
    ZL_SNPRINTF(ip, 128, "%s", inet_ntop(AF_INET, &sockaddr_.sin_addr, tmp, 256));
    return std::string(ip);
}

std::string Socket::getHost()
{
    char host[256], ip[256];
    ZL_SNPRINTF(host, 256, "%s:%d", inet_ntop(AF_INET, &sockaddr_.sin_addr, ip, 256), ntohs(sockaddr_.sin_port));
    return std::string(host);
}


NAMESPACE_ZL_NET_END
