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
#include "net/ByteBuffer.h"
#include "net/TcpConnection.h"
#include "base/Timestamp.h"
#include "base/Logger.h"
#include "thread/Thread.h"
#include "thread/ThreadPool.h"
#include <string>
#include <memory>
#include <thread>
using namespace std;
using namespace zl;
using namespace zl::base;
using namespace zl::thread;
using namespace zl::net;

// epoll如果一个socket正在线程池中被处理，epoll上又收到了该socket的数据 读请求，怎么办？
zl::thread::ThreadPool pool("ThreadPool");;
void do_message(const TcpConnectionPtr& conn, ByteBuffer *buf, Timestamp time)
{
	printf("-------------\n");
    string msg(buf->retrieveAllAsString());
    printf("[thread : %d][%d] recv %d bytes[%s]\n", zl::thread::this_thread::tid(), conn->fd(), msg.size(), msg.c_str());
    printf("==[%p-%p]==\n", conn.get(), buf);
	sleep(10);
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


EchoServer::EchoServer(EventLoop *loop, const InetAddress& listenAddr, int numReactors/* = 0*/)
    : loop_(loop)
{
    server_ = new TcpServer(loop, listenAddr, "EchoServer");
    server_->setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    server_->setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    server_->setMultiReactorThreads(numReactors);
	//pool.start(10);
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

void EchoServer::onMessage(const TcpConnectionPtr& conn, ByteBuffer *buf, const Timestamp& time)
{
	if(1)
	{
		//std::thread trd(std::bind(do_message, conn, buf, time));
		zl::thread::Thread trd(std::bind(do_message, conn, buf, time));
		cout << "trd detach\n";
		trd.detach();
	}
	else
	{
		pool.run(std::bind(do_message, conn, buf, time));
	}
}
