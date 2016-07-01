#include "zlreactor/net/poller/SelectPoller.h"
#include "zlreactor/net/Channel.h"
#include "zlreactor/base/Logger.h"
NAMESPACE_ZL_NET_START

SelectPoller::SelectPoller(EventLoop *loop)
    : Poller(loop)
{
    FD_ZERO(&readfds_);
    FD_ZERO(&writefds_);
    FD_ZERO(&exceptfds_);
	FD_ZERO(&select_readfds_);
    FD_ZERO(&select_writefds_);
    FD_ZERO(&select_exceptfds_);
}

SelectPoller::~SelectPoller()
{

}

bool SelectPoller::updateChannel(Channel *channel)
{
    ZL_SOCKET fd = channel->fd();
    LOG_INFO("SelectPoller::updateChannel[%d]", fd);
	
    FD_SET(fd, &exceptfds_);

	int events = channel->events();
    if(hasChannel(channel))    //exist, update
    {
        assert(getChannel(fd) == channel);

        if(events & FDEVENT_IN)    FD_SET(fd, &select_readfds_);
        else                       FD_CLR(fd, &select_readfds_);

        if(events & FDEVENT_OUT)   FD_SET(fd, &select_writefds_);
        else                       FD_CLR(fd, &select_writefds_);

        if (channel->isNoneEvent())
        {
            LOG_INFO("SelectPoller::updateChannel [%d][%0x] NoneEvent", fd, channel);
			fdlist_.erase(fd);
        }
    }
    else                       //new, add
    {
        assert(getChannel(fd) == NULL);

        if (events & FDEVENT_IN)   FD_SET(fd, &select_readfds_);
        if (events & FDEVENT_OUT)  FD_SET(fd, &select_writefds_);

        channelMap_[fd] = channel;
	    fdlist_.insert(fd);
        //channelIter_.insert(std::make_pair(channel, pollfds_.size() - 1));
    }
    return true;
}

bool SelectPoller::removeChannel(Channel *channel)
{
    if(!hasChannel(channel))
        return true;

    ZL_SOCKET fd = channel->fd();
    LOG_INFO("SelectPoller::removeChannel [%d][%0x]", fd, channel);
    assert(hasChannel(channel) && "the remove socket must be already exist");
    assert(getChannel(fd) == channel && "the remove socket must be already exist");
    assert(channel->isNoneEvent());
    size_t n = channelMap_.erase(fd);
    ZL_UNUSED(n);
    assert(n == 1);

    fdlist_.erase(fd);

    FD_CLR(fd, &select_readfds_);
    FD_CLR(fd, &select_writefds_);
	FD_CLR(fd, &select_exceptfds_);

	return true;
}

Timestamp SelectPoller::pollOnce(int timeoutMs, ChannelList& activeChannels)
{	
	struct timeval tv = {0, 0};
	struct timeval *ptv = &tv;
	if(timeoutMs < 0)
        ptv = NULL;
	else
	{
	    tv.tv_sec =  timeoutMs / 1000;
	    tv.tv_usec = (timeoutMs % 1000) * 1000;
	}

	readfds_   = select_readfds_;
	writefds_  = select_writefds_;
	exceptfds_ = select_exceptfds_;

    //LOG_INFO("SelectPoller::pollOnce: [%d][%d]", *fdlist_.begin(), timeoutMs);
    int numEvents = ::select(*fdlist_.begin() + 1, &readfds_, &writefds_, &exceptfds_, ptv);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if (numEvents > 0)
    {
		LOG_INFO("SelectPoller::pollOnce: events happended[%d]", numEvents);
        fireActiveChannels(numEvents, activeChannels);
    }
    else if (numEvents == 0)
    {
        LOG_INFO("SelectPoller::pollOnce: nothing happended");
    }
    else
    {
        if (savedErrno != SOCK_ERR_EINTR)
        {
            errno = savedErrno;
            LOG_INFO("SelectPoller::pollOnce: error [%d]", errno);
        }
    }
    return now;
}

void SelectPoller::fireActiveChannels(int numEvents, ChannelList& activeChannels) const
{
    //for (ZL_SOCKET fd = 0; numEvents > 0 && fd <= maxFd_ ; ++fd) //可优化，此处再单独保存一个fd的集合即可
	for(auto it = fdlist_.begin(); numEvents > 0 && it != fdlist_.end(); ++it)
    {
		ZL_SOCKET fd = *it;

		int revents = FDEVENT_NONE;
		if (FD_ISSET(fd, &readfds_))	 revents |= FDEVENT_IN;
		if (FD_ISSET(fd, &writefds_))    revents |= FDEVENT_OUT;
		if (FD_ISSET(fd, &exceptfds_))   revents |= FDEVENT_ERR;

        if (revents != FDEVENT_NONE)
        {
            --numEvents;
            Channel *channel = getChannel(fd);
            assert(channel && "the channel must be already exist");

			channel->set_revents(revents);
			activeChannels.push_back(channel);
        }
    }
}

NAMESPACE_ZL_NET_END
