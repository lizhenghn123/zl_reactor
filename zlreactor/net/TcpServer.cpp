#include "net/TcpServer.h"
#include "net/InetAddress.h"
#include "base/ZLog.h"
#include "net/Acceptor.h"
#include "net/TcpConnection.h"
#include "net/EventLoop.h"
using namespace zl::base;
NAMESPACE_ZL_NET_START

TcpServer::TcpServer(EventLoop *loop, const InetAddress& listenAddr, const std::string& server_name/* = "TcpServer"*/)
    : loop_(loop), serverAddr_(listenAddr.getSockAddrInet()), serverName_(server_name)
{
    acceptor_ = new Acceptor(loop, listenAddr);
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, 
                this, std::placeholders::_1, std::placeholders::_2));

    connectionCallback_ = defaultConnectionCallback;
    messageCallback_ = defaultMessageCallback;
}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
    loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_));
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
    loop_->assertInLoopThread();
    EventLoop* ioLoop = loop_; //loopthreadPool_->getNextLoop();

    LOG_INFO("TcpServer::newConnection [%d] from [%s]", sockfd, peerAddr.ipPort().c_str());
    InetAddress localAddr(SocketUtil::getLocalAddr(sockfd));
    TcpConnectionPtr conn  = new TcpConnection(ioLoop, sockfd, localAddr, peerAddr);
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1)); // FIXME: unsafe    
    
    connections_[sockfd] = conn;
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    LOG_INFO("TcpServer::removeConnectionInLoop [%d] - connection %s", conn->fd(), conn->peerAddress().ipPort().c_str());
    size_t n = connections_.erase(conn->fd());
    (void)n;
    assert(n == 1);

    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

NAMESPACE_ZL_NET_END