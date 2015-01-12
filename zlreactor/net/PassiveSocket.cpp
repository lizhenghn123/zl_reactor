#include "net/PassiveSocket.h"

NAMESPACE_ZL_NET_START

PassiveSocket::PassiveSocket(ZL_SOCKET fd) : Socket(fd)
{

}

PassiveSocket::PassiveSocket(const char *ip, int port) : Socket(SocketUtil::createSocket())
{
    if(!Socket::bind(ip, port))
    {
        throw SocketException("Could not bind to port.");
    }
    if(!Socket::listen(5))
    {
        throw SocketException("Could not listen to port.");
    }
}

PassiveSocket::~PassiveSocket()
{
}

const PassiveSocket& PassiveSocket::operator << (const std::string& data) const
{
    if(!Socket::send(data))
    {
        throw SocketException("Could not write to socket.");
    }

    return *this;
}

const PassiveSocket& PassiveSocket::operator >> (std::string& data) const
{
    if(!Socket::recv(data))
    {
        throw SocketException("Could not read from socket.");
    }

    return *this;
}

NAMESPACE_ZL_NET_END
