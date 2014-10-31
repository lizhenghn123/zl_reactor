#include "net/Poller.h"
#include "net/Channel.h"
NAMESPACE_ZL_NET_START

Poller::Poller(EventLoop *loop) : loop_(loop)
{
}

Poller::~Poller()
{
}

bool Poller::hasChannel(const Channel* channel) const
{
    ChannelMap::const_iterator itr = channelMap_.find(channel->fd());
    return itr != channelMap_.end() && itr->second == channel;
}

Channel* Poller::getChannel(ZL_SOCKET sock) const
{
    ChannelMap::const_iterator itr = channelMap_.find(sock);
    if(itr == channelMap_.end())
        return NULL;
    return itr->second;
}

NAMESPACE_ZL_NET_END