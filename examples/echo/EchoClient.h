/*************************************************************************
	File Name   : EchoClient.h
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年05月12日 星期二 20时19分49秒
 ************************************************************************/
#include <string>
#include "net/CallBacks.h"
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
        class TcpClient;
        class Channel;
        class Socket;
        class NetBuffer;
    }
}
using namespace zl;
using namespace zl::net;

class EchoClient
{
public:
    EchoClient(EventLoop *loop, const InetAddress& serverAddr, const std::string& name = "EchoClient");
    ~EchoClient();

    int fd() const;

    void connect();
    void stop();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, NetBuffer *buf, const Timestamp& time);

private:
    EventLoop *loop_;
    TcpClient *client_;
    bool      running_;  // unsafe, use atomic
};