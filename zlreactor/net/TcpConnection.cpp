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

void defaultMessageCallback(TcpConnectionPtr conn, NetBuffer* buf, Timestamp receiveTime)
{
    LOG_INFO("defaultMessageCallback : [%d][%s]", conn->fd(), buf->toString().c_str());
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
    //std::string ttt(data, data+len)
    //send(ttt);
}

void TcpConnection::send(const std::string& buffer)
{
      send(buffer.data(), buffer.size());
}

void TcpConnection::send(NetBuffer* buffer)
{
    if (state_ == kConnected)
    {
        if (loop_->isInLoopThread())
        {
            sendInLoop(buffer->peek(), buffer->readableBytes());
            buffer->retrieveAll();
        }
        else
        {
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop2, this, buffer->retrieveAllAsString()));
            //loop_->runInLoop(std::bind(&TcpConnection::sendInLoop2, this, "ddd", 23));
        }
    }
}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
    loop_->assertInLoopThread();
    if (state_ == kDisconnected)
    {
        LOG_WARN("TcpConnection::sendInLoop [%d]disconnected, give up writing", socket_->fd());
        return;
    }
    
    size_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    // if no thing in output queue, try writing directly
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
    {
        nwrote = socket_->send((const char*)data, len);
        if (nwrote >= 0)
        {
            remaining = len - nwrote;
            if (remaining == 0 && writeCompleteCallback_)
            {
                loop_->queueInLoop(std::bind(writeCompleteCallback_, this));
            }
        }
        else // nwrote < 0
        {
            nwrote = 0;
            if (errno != EWOULDBLOCK)
            {
                LOG_ERROR("TcpConnection::sendInLoop error, fd[%d], error[%d]", socket_->fd(), errno);
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    faultError = true;
                }
            }
        }
    }

    assert(remaining <= len);
    if (!faultError && remaining > 0)
    {
        outputBuffer_.write(static_cast<const char*>(data) + nwrote, remaining);
        if (!channel_->isWriting())
        {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::sendInLoop2(const std::string& buffer)
{
    sendInLoop(buffer.data(), buffer.size());
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
    loop_->assertInLoopThread();
    if (!channel_->isWriting())
    {
        SocketUtil::shutdownWrite(socket_->fd());   // we are not writing
    }
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
    inputBuffer_.write(data);
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
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    LOG_INFO("TcpConnection::handleWrite fd = %d, state = %d", socket_->fd(), state_);
    loop_->assertInLoopThread();

    if (channel_->isWriting())
    {
        size_t n = socket_->send(outputBuffer_.peek(), outputBuffer_.readableBytes());
        if (n > 0)
        {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0)
            {
                channel_->disableWriting();
                if (writeCompleteCallback_)
                {
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, this));
                }
                if (state_ == kDisconnecting)
                {
                    shutdownInLoop();
                }
            }
        }
        else
        {
            LOG_ERROR("TcpConnection::handleWrite, send fail fd = %d, state = %d, send = %d", socket_->fd(), state_, n);
             if (state_ == kDisconnecting)
             { 
                 shutdownInLoop();
             }
        }
    }
    else
    {
        LOG_ERROR("TcpConnection::handleWrite,  no more writing, fd = %d, state = %d", socket_->fd(), state_);
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