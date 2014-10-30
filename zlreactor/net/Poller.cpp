#include "net/Poller.h"
//#include "Channel.h"
NAMESPACE_ZL_NET_START

Poller::Poller(EventLoop *loop) : loop_(loop)
{
}

Poller::~Poller()
{
}

bool Poller::hasChannel(Channel* channel) const
{
	//ChannelMap::iterator itr = channelMap_.find(channel->fd());
    //return it != channels_.end() && it->second == channel;
	return true;
}

Channel* Poller::getChannel(ZL_SOCKET sock)
{
    ChannelMap::iterator itr = channelMap_.find(sock);
    if(itr == channelMap_.end())
        return NULL;
    return itr->second;
}

NAMESPACE_ZL_NET_END

