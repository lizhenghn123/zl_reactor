#include "net/EpollPoller.h"
#include <string.h>
#include <sys/epoll.h>
#include "net/Socket.h"
#include "net/Channel.h"
#include "base/ZLog.h"
using namespace zl::base;
NAMESPACE_ZL_NET_START

EpollPoller::EpollPoller(EventLoop *loop, bool enableET/* = false*/)
    : Poller(loop), enableET_(enableET), events_(64)
{
    epollfd_ = epoll_create(1024);
    assert(epollfd_ > 0 && " epoll create failure!");
}

EpollPoller::~EpollPoller()
{
    ::close(epollfd_);
}

Timestamp EpollPoller::poll_once(int timeoutMs, ChannelList& activeChannels)
{
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(),
                               static_cast<int>(events_.size()), timeoutMs);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if (numEvents > 0)
    {
        LOG_INFO("poll: [%s] events happended", numEvents);
        //fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size()*2);
        }
    }
    else if (numEvents == 0)
    {
        LOG_INFO("poll: nothing happended");
    }
    else
    {
        // error happens, log uncommon ones
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
            LOG_INFO("poll: error [%d]", savedErrno);
        }
    }

    return now;
}

void EpollPoller::fireActiveChannels(int numEvents, ChannelList& activeChannels) const
{
    assert(static_cast<size_t>(numEvents) <= events_.size());
    for (int i = 0; i < numEvents; ++i)
    {
        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
        assert(hasChannel(channel)&& "the poll socket must be already exist");
        channel->set_revents(events_[i].events);
        activeChannels.push_back(channel);
    }
}

bool EpollPoller::updateChannel(Channel *channel)
{
    ZL_SOCKET fd = channel->fd();
    if(hasChannel(channel))    //exist, update
    {
        assert(getChannel(fd) == channel);
        if(channel->isNoneEvent())
        {
            channelMap_.erase(fd);
            return update(channel, EPOLL_CTL_DEL);
        }
        else
        {
            return update(channel, EPOLL_CTL_MOD);
        }
    }
    else                       //new, add
    {
        assert(getChannel(fd) == NULL);
        channelMap_[fd] = channel;
        return update(channel, EPOLL_CTL_ADD);
    }
}

bool EpollPoller::removeChannel(Channel *channel)
{
    ZL_SOCKET fd = channel->fd();
    assert(hasChannel(channel) && "the remove socket must be already exist");
    assert(getChannel(fd) == channel && "the remove socket must be already exist");
    assert(channel->isNoneEvent());
    size_t n = channelMap_.erase(fd);
    (void)n;
    assert(n == 1);

    return update(channel, EPOLL_CTL_DEL);
}

bool EpollPoller::update(Channel *channel, int operation)
{
    ZL_SOCKET fd = channel->fd();
    struct epoll_event ev = { 0, { 0 } };
    ev.events = channel->events();
    ev.data.ptr = channel;
    if (::epoll_ctl(epollfd_, operation, fd, &ev) < 0)
    {
        LOG_CRITICA("EpollPoller::update error, [socket %d][op %d]", fd, operation);
        return false;
    }
    return true;
}

NAMESPACE_ZL_NET_END
