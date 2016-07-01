#include "zlreactor/net/poller/Poller.h"
#include "zlreactor/net/Channel.h"
#include "zlreactor/net/poller/EpollPoller.h"
#include "zlreactor/net/poller/PollPoller.h"
#include "zlreactor/net/poller/SelectPoller.h"
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

/*static*/ Poller* Poller::createPoller(EventLoop *loop)
{
#if defined(USE_POLLER_EPOLL)
	return new EpollPoller(loop);
#elif defined(USE_POLLER_SELECT)
    return new SelectPoller(loop);
#elif defined(USE_POLLER_POLL)
    return new PollPoller(loop);
#else
	return NULL;
#endif
}

NAMESPACE_ZL_NET_END