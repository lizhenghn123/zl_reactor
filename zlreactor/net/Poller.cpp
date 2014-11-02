#include "net/Poller.h"
#include "net/Channel.h"
#include "net/EpollPoller.h"
NAMESPACE_ZL_NET_START

Poller::Poller(EventLoop *loop) : loop_(loop)
{
}

Poller::~Poller()
{
}

bool Poller::hasChannel(const Channel* channel) const
{
    for (ChannelMap::const_iterator itr = channelMap_.begin(); itr!=channelMap_.end(); ++itr)
    {
        printf("=====[%d][%0x]\n", itr->first, itr->second);
    }
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

/*static*/ Poller* Poller::createPoller(EventLoop *loop)
{
    return  new EpollPoller(loop);
}

NAMESPACE_ZL_NET_END