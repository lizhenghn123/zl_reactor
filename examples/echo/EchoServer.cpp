/*************************************************************************
File Name   : EchoServer.cpp
Author      : LIZHENG
Mail        : lizhenghn@gmail.com
Created Time: 2015年05月12日 星期二 19时44分29秒
************************************************************************/
#include "EchoServer.h"
#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/NetBuffer.h"
#include "net/TcpConnection.h"
#include "base/Timestamp.h"
#include "base/Logger.h"
#include <string>
#include <memory>
using namespace std;
using namespace zl;
using namespace zl::base;
using namespace zl::net;

EchoServer::EchoServer(EventLoop *loop, const InetAddress& listenAddr, int numReactors/* = 0*/)
    : loop_(loop)
{
    server_ = new TcpServer(loop, listenAddr, "EchoServer");
    server_->setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    server_->setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    server_->setMultiReactorThreads(numReactors);
}

EchoServer::~EchoServer()
{
    delete server_;
}

void EchoServer::start()
{
    server_->start();
}

void EchoServer::onConnection(const TcpConnectionPtr& conn)
{
    LOG_INFO("%s <-> %s is %s", conn->peerAddress().ipPort().c_str(), conn->localAddress().ipPort().c_str(), (conn->connected() ? "UP" : "DOWN"));
    if (conn->connected())
    {
        conn->send("hello world\n");
    }
    else
    {
        conn->shutdown();
    }
}

void EchoServer::onMessage(const TcpConnectionPtr& conn, NetBuffer *buf, const Timestamp& time)
{
    string msg(buf->retrieveAllAsString());
    LOG_INFO("[%d] recv %d bytes[%s]", conn->fd(), msg.size(), msg.c_str());

    if (msg == "bye\n")
    {
        conn->send("ok\n");
        conn->shutdown();
    }
    else
    {
        conn->send(msg);
    }
}
