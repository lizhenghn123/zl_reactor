#include "net/TcpServer.h"
#include "net/InetAddress.h"
#include "base/ZLog.h"
#include "net/TcpAcceptor.h"
#include "net/TcpConnection.h"
#include "net/EventLoop.h"
#include "net/EventLoopThreadPool.h"
NAMESPACE_ZL_NET_START

TcpServer::TcpServer(EventLoop *loop, const InetAddress& listenAddr, const std::string& server_name/* = "TcpServer"*/)
    : loop_(loop), serverAddr_(listenAddr.getSockAddrInet()), serverName_(server_name)
{
    acceptor_ = new TcpAcceptor(loop, listenAddr);
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, 
                            this, std::placeholders::_1, std::placeholders::_2));

    connectionCallback_ = defaultConnectionCallback;
    messageCallback_ = defaultMessageCallback;

    evloopThreadPool_ = new EventLoopThreadPool(loop_);
}

TcpServer::~TcpServer()
{
    loop_->assertInLoopThread();

    for (ConnectionMap::iterator it(connections_.begin()); it != connections_.end(); ++it)
    {
        TcpConnectionPtr conn = it->second;
        it->second.reset();
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
        conn.reset();
    }
}

void TcpServer::setThreadNum(size_t numThreads)
{
    if(numThreads <= 0)
        numThreads = zl::thread::Thread::hardware_concurrency();
    evloopThreadPool_->setThreadNum(numThreads);
}

void TcpServer::start()
{
    evloopThreadPool_->start();
    loop_->runInLoop(std::bind(&TcpAcceptor::listen, acceptor_));
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
    loop_->assertInLoopThread();
    EventLoop* ioLoop = evloopThreadPool_->getNextLoop();  // loop_;

    LOG_INFO("TcpServer::newConnection [%d] from [%s]", sockfd, peerAddr.ipPort().c_str());
    InetAddress localAddr(SocketUtil::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(ioLoop, sockfd, localAddr, peerAddr));
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    
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
    LOG_INFO("TcpServer::removeConnectionInLoop [%d] - %s", conn->fd(), conn->peerAddress().ipPort().c_str());
    size_t n = connections_.erase(conn->fd());
    ZL_UNUSED(n);
    assert(n == 1);

    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    LOG_INFO("TcpServer::removeConnectionInLoop [%d] - %s", conn->fd(), conn->peerAddress().ipPort().c_str());
}

NAMESPACE_ZL_NET_END
