#include "net/Acceptor.h"
#include "net/Socket.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "base/ZLog.h"
#include "net/ActiveSocket.h"
#include "net/InetAddress.h"
using namespace zl::base;
NAMESPACE_ZL_NET_START

Acceptor::Acceptor(EventLoop *loop, const InetAddress& listenAddr)
    : loop_(loop)
{
    accept_socket = new Socket(SocketUtil::createSocket());

    accept_socket->setNoDelay();
    accept_socket->setNonBlocking();

    if (!accept_socket->setReuseAddr(true))
    {
        throw SocketException("Could not reuse socket address.");
    }
    if (!accept_socket->bind(listenAddr))
    {
        throw SocketException("Could not bind to port.");
    }

    accept_channel_ = new Channel(loop, accept_socket->fd());
    accept_channel_->setReadCallback(std::bind(&Acceptor::onAccept, this, std::placeholders::_1));
}

Acceptor::~Acceptor()
{
    accept_channel_->disableAll();
    accept_channel_->remove();
    Safe_Delete(accept_channel_);
    Safe_Delete(accept_socket);
}

void Acceptor::listen()
{
    loop_->assertInLoopThread();
    if (!accept_socket->listen(128)) //may be bigger, see 'cat /proc/sys/net/core/somaxconn'
    {
        throw SocketException("Could not listen to port.");
    }
    LOG_INFO("Acceptor::listen on [%s]", accept_socket->getHost().c_str());

    accept_channel_->enableReading();
}

void Acceptor::onAccept(Timestamp now)
{
    loop_->assertInLoopThread();
    while (true)
    {
        InetAddress peerAddr;
        ZL_SOCKET newfd = accept_socket->accept(&peerAddr);
        if(newfd > 0)
        {
            if (newConnCallBack_)
            {  
                LOG_INFO("Acceptor::OnAccept accept one client from[%d][%s]", newfd, peerAddr.ipPort().c_str());
                newConnCallBack_(newfd, peerAddr);
            }
            else
            {
                LOG_ALERT("Acceptor::OnAccept() no callback , and close the coming connection![%d]", newfd);
            }
        }
        else
        {
            LOG_ALERT("Acceptor::OnAccept() accept connection fail![%d][%d]", newfd, errno);
            //if (errno == )
            //EAGAIN：套接字处于非阻塞状态，当前没有连接请求。
            //	EBADF：非法的文件描述符。
            //	ECONNABORTED：连接中断。
            //	EINTR：系统调用被信号中断。
            //	EINVAL：套接字没有处于监听状态，或非法的addrlen参数。
            //	EMFILE：达到进程打开文件描述符限制。
            //	ENFILE：达到打开文件数限制。
            //	ENOTSOCK：文件描述符为文件的文件描述符。
            //	EOPNOTSUPP：套接字类型不是SOCK_STREAM。
            break;
        }
    }
}

NAMESPACE_ZL_NET_END
