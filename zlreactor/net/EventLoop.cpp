#include "net/EventLoop.h"
#include <assert.h>
#include "net/Channel.h"
#include "net/Poller.h"
#include "base/Timestamp.h"
#include "base/ZLog.h"
using namespace zl::base;
NAMESPACE_ZL_NET_START

EventLoop::EventLoop() : looping_(false), quit_(false)
{
    poller_ = Poller::createPoller(this);
}

EventLoop::~EventLoop()
{
     Safe_Delete(poller_);
}

void EventLoop::loop()
{
    Timestamp retime;
    while (true)
    {
        activeChannels_.clear();
        retime = poller_->poll_once(10000, activeChannels_);
        //LOG_INFO("EventLoop::loop [%s][%d]", retime.toString().c_str(), activeChannels_.size());
        for (ChannelList::iterator it = activeChannels_.begin(); it != activeChannels_.end(); ++it)
        {
            currentActiveChannel_ = *it;
            currentActiveChannel_->handleEvent(retime);
        }
        currentActiveChannel_ = NULL;
        //doPendingFunctors();
    }
}

void EventLoop::quit()
{

}

void EventLoop::wakeup()
{

}

void EventLoop::updateChannel(Channel* channel)
{
	LOG_INFO("EventLoop::updateChannel [%d]", channel->fd());
	//assert(channel->ownerLoop() == this);
	//assertInLoopThread();
	poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
	//assert(channel->ownerLoop() == this);
	//assertInLoopThread();
	if (eventHandling_)
	{
		assert(currentActiveChannel_ == channel ||
			std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
	}
	poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
    return poller_->hasChannel(channel);
}

void EventLoop::runInLoop(const Functor& func)
{
    LOG_INFO("EventLoop::runInLoop [%d]", isInLoopThread());
	if (isInLoopThread())
	{
		func();
	}
	else
	{
		queueInLoop(func);
	}
}

void EventLoop::queueInLoop(const Functor& func)
{
	//zl::thread::MutexLocker lock(mutex_);

}

NAMESPACE_ZL_NET_END