#include "net/TcpAcceptor.h"
#include "base/Exception.h"
#include "base/Logger.h"
#include "net/Socket.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
NAMESPACE_ZL_NET_START

TcpAcceptor::TcpAcceptor(EventLoop *loop, const InetAddress& listenAddr)
    : loop_(loop)
{
    accept_socket = new Socket(SocketUtil::createSocket());

    accept_socket->setNoDelay();
    accept_socket->setNonBlocking();

    if (!accept_socket->setReuseAddr(true))
    {
        throw zl::base::Exception("Could not reuse socket address.");
    }
    if (!accept_socket->bind(listenAddr))
    {
        throw zl::base::Exception("Could not bind to port.");
    }

    accept_channel_ = new Channel(loop, accept_socket->fd());
    accept_channel_->setReadCallback(std::bind(&TcpAcceptor::onAccept, this, std::placeholders::_1));
}

TcpAcceptor::~TcpAcceptor()
{
    accept_channel_->disableAll();
    accept_channel_->remove();
    Safe_Delete(accept_channel_);
    Safe_Delete(accept_socket);
}

void TcpAcceptor::listen()
{
    loop_->assertInLoopThread();
    if (!accept_socket->listen(128)) //may be bigger, see 'cat /proc/sys/net/core/somaxconn'
    {
        throw zl::base::Exception("Could not listen to port.");
    }
    LOG_INFO("TcpAcceptor::listen on [%s]", SocketUtil::getLocalIpPort(accept_socket->fd()).c_str());

    accept_channel_->enableReading();
}

void TcpAcceptor::onAccept(Timestamp now)
{
    loop_->assertInLoopThread();
    int count = 0;
    while(count < 100)
    {
        InetAddress peerAddr;
        ZL_SOCKET newfd = accept_socket->accept(&peerAddr);
        if(newfd > 0)
        {
            if (newConnCallBack_)
            {  
                LOG_INFO("TcpAcceptor::OnAccept accept one client from[%d][%s]", newfd, peerAddr.ipPort().c_str());
                newConnCallBack_(newfd, peerAddr);
            }
            else
            {
                LOG_ALERT("TcpAcceptor::OnAccept() no callback , and close the coming connection![%d]", newfd);
                SocketUtil::closeSocket(newfd);
            }
            count ++;
        }
        else
        {
            if(ZL_SOCKET_ERROR == SOCK_ERR_EAGAIN || ZL_SOCKET_ERROR == SOCK_ERR_EWOULDBLOCK)
            {
                //We have processed all incoming  connections.
            }
            else if(ZL_SOCKET_ERROR == SOCK_ERR_EMFILE)
            {
                // TODO 此时因为达到最大文件描述符而接收失败，因为poller使用的是水平触发模式，
                // 会导致poller持续通知可读事件，因此造成acceptor频繁去accept，直至进程中关闭了
                // 其他连接而有空余描述符才停止。这样会导致CPU 100% loop。
                // 解决方案见 ： http://blog.csdn.net/solstice/article/details/6365666
                // http://pod.tst.eu/http://cvs.schmorp.de/libev/ev.pod#The_special_problem_of_accept_ing_wh
            }
            else
            {
                LOG_ALERT("TcpAcceptor::OnAccept() accept connection error![%d][%d]", newfd, errno);
            }
            break;
        }
    }
}

NAMESPACE_ZL_NET_END
