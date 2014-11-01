#include "net/Acceptor.h"
#include "net/Socket.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "base/ZLog.h"
#include "net/ActiveSocket.h"
using namespace zl::base;
NAMESPACE_ZL_NET_START

Acceptor::Acceptor(EventLoop *loop, const InetAddress& listenAddr)
    : loop_(loop)
{
    accept_socket = new Socket();
    if (!accept_socket->create())
    {
        throw SocketException("Could not create server socket.");
    }
    accept_socket->setNoDelay();

    if (!accept_socket->setReuseAddr(true))
    {
        throw SocketException("Could not reuse socket address.");
    }
    if (!accept_socket->bind(listenAddr))
    {
        throw SocketException("Could not bind to port.");
    }

    accept_channel_ = new Channel(loop, accept_socket->getSocket());
    accept_channel_->setReadCallback(std::bind(&Acceptor::onAccept, this, std::placeholders::_1));
}

Acceptor::~Acceptor()
{
    accept_socket->close();
    Safe_Delete(accept_channel_);
    Safe_Delete(accept_socket);
}

void Acceptor::listen()
{
    if (!accept_socket->listen(64))
    {
        throw SocketException("Could not listen to port.");
    }
    LOG_INFO("server host [%s]", accept_socket->getHost().c_str());

    accept_channel_->enableReading();
}

void Acceptor::onAccept(Timestamp now)
{
    while (true)
    {
        ActiveSocket *socket = new ActiveSocket;
        if (accept_socket->accept(*socket))
        {
            if (newConnCallBack_)
            {
                newConnCallBack_(socket);
            }
            else
            {
                LOG_ALERT("no callback on Acceptor::OnAccept(), and close the coming connection!");
                socket->close();
                Safe_Delete(socket);
            }
            break;
        }
        else
        {
            LOG_ALERT("accept connection fail on Acceptor::OnAccept()!");
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