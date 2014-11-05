#include "net/Connector.h"
#include "net/EventLoop.h"
#include "base/ZLog.h"
#include "net/Channel.h"
using namespace zl::base;
NAMESPACE_ZL_NET_START

Connector::Connector(EventLoop *loop, const InetAddress& serverAddr)
    : state_(kDisconnected), connect_(false), 
      loop_(loop), serverAddr_(serverAddr),
      connector_channel_(NULL)
{

}

Connector::~Connector()
{

}

void Connector::connect()
{
     connect_ = true;
     loop_->runInLoop(std::bind(&Connector::connectInLoop, this));
}

void Connector::connectInLoop()
{
    loop_->assertInLoopThread();
    assert(state_ == kDisconnected);
    if (connect_)
    {
        connectServer();
    }
}

void Connector::connectServer()
{
    ZL_SOCKET sockfd = SocketUtil::createSocket(); //::createNonblockingOrDie();
    SocketUtil::setNonBlocking(sockfd);

    int ret = SocketUtil::connect(sockfd, serverAddr_.getSockAddrInet());
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno)
    {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
        connectEstablished(sockfd);
        break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
        //retry(sockfd);
        break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
        LOG_ERROR("Connector::connectServer() error[%d]", savedErrno);
        SocketUtil::closeSocket(sockfd);
        break;

    default:
        LOG_ERROR("Connector::connectServer() unexpected error[%d]", savedErrno);
        SocketUtil::closeSocket(sockfd);
        // connectErrorCallback_();
        break;
    }
}

void Connector::connectEstablished(ZL_SOCKET sock)
{
    setState(kConnecting);
    assert(!connector_channel_);
    connector_channel_ = new Channel(loop_, sock);

    connector_channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
    connector_channel_->setErrorCallback(std::bind(&Connector::handleError, this));

    connector_channel_->enableWriting();
}

void Connector::stop()
{
    connect_ = false;
    loop_->queueInLoop(std::bind(&Connector::stopInLoop, this));
}

void Connector::stopInLoop()
{
    loop_->assertInLoopThread();
    if (state_ == kConnecting)
    {
        setState(kDisconnected);
        ZL_SOCKET sockfd = disableChannel();
        retry(sockfd);
    }
}

ZL_SOCKET Connector::disableChannel()
{
    connector_channel_->disableAll();   // 从poller中移除，不再关注任何事件
    connector_channel_->remove();
    ZL_SOCKET sockfd = connector_channel_->fd();
    return sockfd;
}

//连接远端socket成功
void Connector::handleWrite()
{
    if (state_ == kConnecting) //连接建立时注册Channel可写事件，此时响应可写，将socket返回，并禁用Channel
    {
        ZL_SOCKET sockfd = disableChannel();
        int err = SocketUtil::getSocketError(sockfd);
        if (err)
        {
            LOG_WARN("Connector::handleWrite - SO_ERROR = [%d][%d][%s]", sockfd, err, strerror(err));
            retry(sockfd);
        }
        else if (SocketUtil::isSelfConnect(sockfd))
        {
            LOG_WARN("Connector::handleWrite - Self connect = [%d]", sockfd);
            retry(sockfd);
        }
        else
        {
            setState(kConnected);
            if (connect_)
            {
                newConnCallBack_(sockfd);
            }
            else
            {
                SocketUtil::closeSocket(sockfd);
            }
        }
    }
    else
    {
        assert(state_ == kDisconnected);
    }
}

void Connector::handleError()
{
    LOG_ERROR("Connector::handleError(): fd = [%d], state = [%d]", connector_channel_->fd(), state_);
    if (state_ == kConnecting)
    {
        ZL_SOCKET sockfd = disableChannel();
        int err = SocketUtil::getSocketError(sockfd);
        LOG_ERROR("Connector::handleError() SO_ERROR = [%d][%s]", err, strerror(err));
        retry(sockfd);
    }
}

void Connector::retry(ZL_SOCKET sockfd)
{
    SocketUtil::closeSocket(sockfd);
    setState(kDisconnected);
    if (connect_)
    {
        LOG_INFO("Connector::retry; Retry connecting to [%s]", serverAddr_.ipPort().c_str());
        loop_->runInLoop(std::bind(&Connector::connectInLoop, this));
    }
    else
    {
         LOG_INFO("Connector::retry: do not connect");
    }
}

NAMESPACE_ZL_NET_END