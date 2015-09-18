/*************************************************************************
	File Name   : EchoClient.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年05月12日 星期二 20时19分55秒
 ************************************************************************/
#include "EchoClient.h"
#include "net/TcpClient.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/ByteBuffer.h"
#include "net/TcpConnection.h"
#include "base/Timestamp.h"
#include "base/Logger.h"
#include <string>
#include <memory>

using namespace std;
using namespace zl;
using namespace zl::base;
using namespace zl::net;


EchoClient::EchoClient(EventLoop *loop, const InetAddress& serverAddr, const std::string& name/* = "EchoClient"*/)
    : loop_(loop)
{
    client_ = new TcpClient(loop, serverAddr, name);
    client_->setConnectionCallback(std::bind(&EchoClient::onConnection, this, std::placeholders::_1));
    client_->setMessageCallback(std::bind(&EchoClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

EchoClient::~EchoClient()
{
    delete client_;
}

void EchoClient::connect()
{
    client_->enableRetry();
    client_->connect();
    running_ = true;
}

int EchoClient::fd() const
{
    return client_->fd();
}

void EchoClient::stop()
{
    running_ = false;
}

void EchoClient::onConnection(const TcpConnectionPtr& conn)
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

void EchoClient::onMessage(const TcpConnectionPtr& conn, ByteBuffer *buf, const Timestamp& time)
{
    string msg(buf->retrieveAllAsString());
    LOG_INFO("[%d] recv %d bytes[%s]", conn->fd(), msg.size(), msg.c_str());

    if (running_ == false)
    {
        conn->send("bye\n");
    }
    else if (msg == "ok\n")
    {
        conn->shutdown();
    }
    else
    {
        conn->send(msg);
    }
}
