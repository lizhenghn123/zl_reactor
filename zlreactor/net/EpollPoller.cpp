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

bool EpollPoller::updateChannel(Channel *channel/*, bool enableRead, bool enableWrite*/)
{
/*    channel->ClearChannel();
    ZL_SOCKET sock = channel->GetSocket()->GetSocket();

    struct epoll_event ev = { 0, { 0 } };
    //ev.data.ptr = channel;
    ev.data.fd = sock;

    ev.events = 0;
    if(enableRead)
        ev.events |= EPOLLIN;
    if(enableWrite)
        ev.events |= EPOLLOUT;
    if(enableET_)
        ev.events |= EPOLLET;

    ev.events |= EPOLLERR;
    ev.events |= EPOLLHUP;

    channelMap_[sock] = channel;

    bool rc = (epoll_ctl(epollfd_, EPOLL_CTL_ADD, channel->GetSocket()->GetSocket(), &ev) == 0);
    return rc; */
	return true;
}

bool EpollPoller::removeChannel(Channel *channel)
{
/*    channel->ClearChannel();
    ZL_SOCKET sock = channel->GetSocket()->GetSocket();
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = 0;
    ev.data.fd = 0;//channel->GetSocket()->GetSocket();
    ev.events = 0;

    ChannelMap::iterator itr = channelMap_.find(sock);
    if(itr != channelMap_.end())
    {
        channelMap_.erase(itr);
    }

    bool rc = (epoll_ctl(epollfd_, EPOLL_CTL_DEL, channel->GetSocket()->GetSocket(), &ev) == 0);
    return rc;  */
	return true;
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList& activeChannels)
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
/*    struct epoll_event epoll_events[MAX_EPOLL_EVENTS];
    int nfds = epoll_wait(epollfd_, epoll_events, MAX_EPOLL_EVENTS, -1); //等待epoll事件的发生
    Channel *channel;
    ZL_SOCKET sock;
    uint32_t events;
    for(int i = 0; i < nfds; ++i)   //处理所发生的所有事件
    {
        events = epoll_events[i].events;
        sock = epoll_events[i].data.fd;
        //channel = epoll_events[i].data.ptr;
        channel = getChannel(sock);
        if(channel == NULL)
        {
            printf("channel == null[%d]\n", sock);
            continue;
        }
        channel->clearChannel();
        if(sock == listenfd_)
        {
            channel->setChannel(SOCKETEVENT_ACCEPT);
        }
        else if(events & EPOLLIN)
        {
            channel->setChannel(SOCKETEVENT_READ);
        }
        else if(events & EPOLLOUT)
        {
            channel->setChannel(SOCKETEVENT_WRITE);
        }
        else if((events & EPOLLRDHUP) || (events & EPOLLERR))
        {
            channel->setChannel(SOCKETEVENT_HUP | SOCKETEVENT_ERROR);
            printf("EPOLLRDHUP[%d]\n", channel->GetChannel());
            //            struct epoll_event ev;
            //            ev.data.ptr = 0;
            //            epoll_ctl(epollfd_, EPOLL_CTL_DEL, sock, &ev);
        }
        if(channel->getChannel())
            activeChannels.push_back(channel);
    }
    return activeChannels.size();
*/
}

NAMESPACE_ZL_NET_END
