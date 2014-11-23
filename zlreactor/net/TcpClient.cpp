#include "net/TcpClient.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/TcpConnection.h"
#include "net/Connector.h"
#include "net/SocketUtil.h"
using namespace zl::base;
NAMESPACE_ZL_NET_START

namespace detail
{
    void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn)
    {
        loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }

    void removeConnector(ConnectorPtr connector)
    {
        Safe_Delete(connector);
    }
}

TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr)
    : loop_(loop),
    connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback),
    retry_(false),
    connect_(true)
{
    connector_ = new Connector(loop, serverAddr);
    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
}

TcpClient::~TcpClient()
{
    detail::removeConnection(loop_, connection_);
    connector_->stop();
    loop_->runInLoop(std::bind(&detail::removeConnector, connector_));
}

void TcpClient::connect()
{
    connect_ = true;
    connector_->connect();
}

void TcpClient::disconnect()
{
    connect_ = false;

    if (connection_)
    {
        connection_->shutdown();
    }
}

void TcpClient::stop()
{
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int sockfd)
{
    loop_->assertInLoopThread();
    InetAddress peerAddr(SocketUtil::getPeerAddr(sockfd));
    InetAddress localAddr(SocketUtil::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(loop_, sockfd, localAddr, peerAddr));

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));
    conn->connectEstablished();

    connection_ = conn;
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    assert(loop_ == conn->getLoop());
    assert(connection_ == conn);

    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

NAMESPACE_ZL_NET_END