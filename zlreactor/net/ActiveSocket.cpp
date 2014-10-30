#include "net/ActiveSocket.h"
NAMESPACE_ZL_NET_START

ActiveSocket::ActiveSocket()
{

}

ActiveSocket::ActiveSocket(const char *host, int port)
{
    if(!Socket::Create())
    {
        throw SocketException("Could not create client socket.");
    }
    if(!Socket::Connect(host, port))
    {
        throw SocketException("Could not Connect to server.");
    }
}

ActiveSocket::~ActiveSocket()
{
}

const ActiveSocket& ActiveSocket::operator << (const std::string& data) const
{
    if(!Socket::Send(data))
    {
        throw SocketException("Could not write to socket.");
    }

    return *this;
}

const ActiveSocket& ActiveSocket::operator >> (std::string& data) const
{
    if(!Socket::Recv(data))
    {
        throw SocketException("Could not read from socket.");
        //printf("Could not read from socket.");
    }

    return *this;
}

NAMESPACE_ZL_NET_END
