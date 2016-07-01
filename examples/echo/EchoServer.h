/*************************************************************************
	File Name   : EchoServer.h
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年05月12日 星期二 19时43分29秒
 ************************************************************************/
#ifndef ZL_ECHOSERVER_H
#define ZL_ECHOSERVER_H
#include "zlreactor/net/CallBacks.h"
namespace zl
{
    namespace base
    {
        class Timestamp;
    }
    namespace net
    {
        class EventLoop;
        class InetAddress;
        class TcpServer;
        class Channel;
        class Socket;
        class ByteBuffer;
    }
}

using zl::net::EventLoop;
using zl::net::InetAddress;
using zl::net::TcpServer;
using zl::net::Channel;
using zl::net::Socket;
using zl::net::ByteBuffer;

using namespace zl;
using namespace zl::net;

class EchoServer
{
public:
    EchoServer(EventLoop *loop, const InetAddress& listenAddr, int numReactors = 0);
    ~EchoServer();

    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, ByteBuffer *buf, const Timestamp& time);

private:
    EventLoop *loop_;
    TcpServer *server_;
};

#endif  /* ZL_ECHOSERVER_H */
