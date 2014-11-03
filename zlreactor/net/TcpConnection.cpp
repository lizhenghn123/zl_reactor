#include "TcpConnection.h"
#include "net/Socket.h"
#include "base/ZLog.h"
#include "net/EventLoop.h"
#include "net/Channel.h"

using namespace zl::base;
NAMESPACE_ZL_NET_START

void defaultConnectionCallback(TcpConnectionPtr conn)
{
  LOG_INFO("defaultConnectionCallback : [%s]<->[%s] [%s]\n", conn->localAddress().ipPort().c_str(),
        conn->peerAddress().ipPort().c_str(), conn->connected() ? "UP" : "DOWN");
}

void defaultMessageCallback(TcpConnectionPtr conn, Buffer* buf, Timestamp receiveTime)
{
    LOG_INFO("defaultMessageCallback : [%d][%d]", conn->fd(), buf->data());
}

TcpConnection::TcpConnection(EventLoop* loop, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr)
    : loop_(loop), state_(kConnecting), localAddr_(localAddr.getSockAddrInet()), peerAddr_(peerAddr.getSockAddrInet())
{
    socket_ = new Socket(sockfd);
    socket_->setKeepAlive(true);
    socket_->setNoDelay(true);
    socket_->setNonBlocking();

    channel_ = new Channel(loop, sockfd);
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
    LOG_INFO("TcpConnection::TcpConnection(), [%d][%0x][%0x]", socket_->fd(), socket_, channel_);
}

TcpConnection::~TcpConnection()
{
    LOG_INFO("TcpConnection::~TcpConnection(), [%d][%0x][%0x]", socket_->fd(), socket_, channel_);
    assert(state_ == kDisconnected);
    Safe_Delete(socket_);
    Safe_Delete(channel_);
}

void TcpConnection::send(const void* data, size_t len)
{
    if (state_ == kConnected)
    {
        if (loop_->isInLoopThread())
        {
            sendInLoop(data, len);
        }
        else
        {
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, data, len));
        }
    }
}

void TcpConnection::send(Buffer* buf)
{
}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
    loop_->assertInLoopThread();
    if (state_ == kDisconnected)
    {
        LOG_WARN("TcpConnection::sendInLoop [%d]disconnected, give up writing", socket_->fd());
        return;
    }

    size_t nwrite = socket_->send((const char*)data, len);
    if(nwrite == len)
    {
         if(writeCompleteCallback_)
             loop_->queueInLoop(std::bind(writeCompleteCallback_, this));
    }
    else if(nwrite == 0)     // may be the peer already closed
    {   
        handleClose();
    }
    else if(nwrite < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            LOG_ERROR("TcpConnection::sendInLoop [%d] send error, write[%d], error[%d]", socket_->fd(), nwrite, errno);
            if (errno == EPIPE || errno == ECONNRESET)
            {
                handleError();
            }
        }
    }
}

void TcpConnection::shutdown()
{
    if (state_ == kConnected)
    {
        setState(kDisconnecting);
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop()
{

}

void TcpConnection::connectEstablished()
{
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->enableReading();

    connectionCallback_(this);
}

void TcpConnection::connectDestroyed()
{
    LOG_INFO("TcpConnection::connectDestroyed fd = %d, state = %d", socket_->fd(), state_);
    loop_->assertInLoopThread();
    if (state_ == kConnected)
    {
        setState(kDisconnected);
        channel_->disableAll();

        connectionCallback_(this);
    }
    SocketUtil::closeSocket(socket_->fd());
    channel_->remove();
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
    LOG_INFO("TcpConnection::handleRead fd = %d, state = %d", socket_->fd(), state_);
    loop_->assertInLoopThread();
    std::string data;
    size_t n = socket_->recv(data);
    if (n > 0)
    {
        messageCallback_(this, &data, receiveTime);
    }
    else if (n == 0)
    {
        handleClose();
    }
    else
    {
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    LOG_INFO("TcpConnection::handleWrite fd = %d, state = %d", socket_->fd(), state_);
    loop_->assertInLoopThread();
    if (channel_->isWriting())
    {
        //std::string data;
        //size_t n = socket_->recv(data);
        //if (n > 0)
        //{
        //    messageCallback_(this, &data, receiveTime);
        //}
        //else if (n == 0)
        //{
        //    handleClose();
        //}
        //else
        //{
        //    handleError();
        //}
    }
    else
    {
        LOG_ERROR("TcpConnection::handleWrite error fd = %d, state = %d", socket_->fd(), state_);
    }
}

void TcpConnection::handleClose()
{
    loop_->assertInLoopThread();
    LOG_INFO("TcpConnection::handleClose fd = %d, state = %d", socket_->fd(), state_);
    assert(state_ == kConnected || state_ == kDisconnecting);
    setState(kDisconnected);
    channel_->disableAll();

    connectionCallback_(this);

    closeCallback_(this);
}

void TcpConnection::handleError()
{
    int err = SocketUtil::getSocketError(channel_->fd());
    LOG_ERROR("TcpConnection::handleError [%d], SO_ERROR = %d", channel_->fd(), err);
}

NAMESPACE_ZL_NET_END