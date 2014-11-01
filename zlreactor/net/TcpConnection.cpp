#include "TcpConnection.h"
#include "net/Socket.h"
#include "base/ZLog.h"
#include "net/EventLoop.h"
#include "net/Channel.h"
using namespace zl::base;

NAMESPACE_ZL_NET_START

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
}

TcpConnection::~TcpConnection()
{
    assert(state_ == kDisconnected);
    Safe_Delete(socket_);
    Safe_Delete(channel_);
}

void TcpConnection::send(const void* data, int len)
{
}

void TcpConnection::send(Buffer* buf)
{
}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
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
    loop_->assertInLoopThread();
    if (state_ == kConnected)
    {
        setState(kDisconnected);
        channel_->disableAll();

        connectionCallback_(this);
    }
    channel_->remove();
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
    loop_->assertInLoopThread();
    //int savedErrno = 0;
    //ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
    std::string data;
    size_t n = socket_->recv(data);
    if (n > 0)
    {
        messageCallback_(this, &inputBuffer_, receiveTime);
    }
    else if (n == 0)
    {
        handleClose();
    }
    else
    {
        //errno = savedErrno;
        //LOG_SYSERR << "TcpConnection::handleRead";
        handleError();
    }
}

void TcpConnection::handleWrite()
{
}

void TcpConnection::handleClose()
{
    loop_->assertInLoopThread();
    LOG_INFO("fd = %d, state = %d", socket_->getSocket(), state_);
    assert(state_ == kConnected || state_ == kDisconnecting);
    // we don't close fd, leave it to dtor, so we can find leaks easily.
    setState(kDisconnected);
    channel_->disableAll();

    //TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(this);
    // must be the last line
    closeCallback_(this);
}

void TcpConnection::handleError()
{
    int err = zl::net::getSocketError(channel_->fd());
    LOG_ERROR("TcpConnection::handleError [%d], SO_ERROR = %d", channel_->fd(), err);
}

NAMESPACE_ZL_NET_END