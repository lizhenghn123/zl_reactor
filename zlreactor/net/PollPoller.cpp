#include "net/PollPoller.h"
#include "net/Channel.h"
#include "base/ZLog.h"
NAMESPACE_ZL_NET_START

PollPoller::PollPoller(EventLoop *loop)
    : Poller(loop)
{

}

PollPoller::~PollPoller()
{

}

bool PollPoller::updateChannel(Channel *channel)
{
    ZL_SOCKET fd = channel->fd();
    LOG_INFO("PollPoller::updateChannel[%d]", fd);
    if(channelIter_.find(channel)!=channelIter_.end())    //exist, update
    {
        assert(getChannel(fd) == channel);
        int idx = channelIter_[channel];
        LOG_INFO("PollPoller::updateChannel  fd2 [%d][%d]", idx, channelIter_[channel]);
        assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));

        struct pollfd& pfd = pollfds_[idx];
        assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent())
        {
            LOG_INFO("PollPoller::updateChannel [%d][%0x][%d] NoneEvent", fd, channel, pfd.events);
            pfd.fd = -channel->fd()-1;
        }
    }
    else                       //new, add
    {
        assert(getChannel(fd) == NULL);
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pollfds_.push_back(pfd);

        channelMap_[fd] = channel;
        channelIter_.insert(std::make_pair(channel, pollfds_.size() - 1));
    }
    return true;
}

bool PollPoller::removeChannel(Channel *channel)
{
    if(!hasChannel(channel))
        return true;

    ZL_SOCKET fd = channel->fd();
    int idx = channelIter_[channel];
    LOG_INFO("PollPoller::removeChannel [%d][%d][%0x]", fd, idx, channel);
    assert(getChannel(fd) == channel && "the remove socket must be already exist");
    assert(channel->isNoneEvent());
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));

    const struct pollfd& pfd = pollfds_[idx];
    UNUSED_STATEMENT(pfd);
    assert(pfd.fd == -channel->fd()-1 && pfd.events == channel->events());

    size_t n = channelMap_.erase(fd);
    UNUSED_STATEMENT(n);
    assert(n == 1);
    if ((idx) == static_cast<int>(pollfds_.size()) - 1) // last one
    {

    }
    else
    {
        int lastfd = pollfds_.back().fd;
        iter_swap(pollfds_.begin()+idx, pollfds_.end()-1);
        if (lastfd < 0)
        {
            lastfd = -lastfd-1;
        }
        channelIter_[getChannel(lastfd)] = idx;
    }

    pollfds_.pop_back();
    channelIter_.erase(channel);

    return true;
}

Timestamp PollPoller::poll_once(int timeoutMs, ChannelList& activeChannels)
{
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if (numEvents > 0)
    {
        fireActiveChannels(numEvents, activeChannels);
    }
    else if (numEvents == 0)
    {
        LOG_INFO("PollPoller::poll_once: nothing happended");
    }
    else
    {
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
            LOG_INFO("EpollPoller::poll_once: error [%d]", errno);
        }
    }
    return now;
}

void PollPoller::fireActiveChannels(int numEvents, ChannelList& activeChannels) const
{
    for (PollFdList::const_iterator it = pollfds_.begin(); numEvents > 0 && it != pollfds_.end() ; ++it)
    {
        if (it->revents > 0)
        {
            --numEvents;
            Channel *channel = getChannel(it->fd);
            assert(channel && "the channel must be already exist");
            channel->set_revents(it->revents);
            activeChannels.push_back(channel);
            //LOG_INFO("PollPoller::fireActiveChannels [%d][%d]", it->fd, it->revents);
        }
    }
}

NAMESPACE_ZL_NET_END