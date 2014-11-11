#include "net/EventLoop.h"
#include <assert.h>
#include <sys/eventfd.h>     // for eventfd
#include "net/Channel.h"
#include "net/Poller.h"
#include "base/Timestamp.h"
#include "base/ZLog.h"
using namespace zl::base;
using namespace zl::thread;
NAMESPACE_ZL_NET_START

namespace
{
    int createEventfd()
    {
        int efd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (efd < 0)
        {
            LOG_ALERT("create eventfd failed when EventLoop::EventLoop");
            assert(efd);
        }
        LOG_INFO("EventLoop::createEventfd [%d]", efd);
        return efd;
    }
}

EventLoop::EventLoop()
    : currentThreadId_(this_thread::get_id()),
      currentActiveChannel_(NULL),
      running_(false),
      eventHandling_(false),
      callingPendingFunctors_(false)
{
    poller_ = Poller::createPoller(this);

    wakeupfd_ = createEventfd();
    wakeupChannel_ = new Channel(this, wakeupfd_);
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();  // ready for read event of wakeupfd_
}

EventLoop::~EventLoop()
{
    Safe_Delete(poller_);
    Safe_Delete(wakeupChannel_);
}

void EventLoop::loop()
{
    running_ = true;

    Timestamp retime;
    while (running_)
    {
        activeChannels_.clear();
        retime = poller_->poll_once(10000, activeChannels_);
        //LOG_INFO("EventLoop::loop [%s][%d]", retime.toString().c_str(), activeChannels_.size());
        eventHandling_ = true;
        for (ChannelList::iterator it = activeChannels_.begin(); it != activeChannels_.end(); ++it)
        {
            currentActiveChannel_ = *it;
            currentActiveChannel_->handleEvent(retime);
        }
        currentActiveChannel_ = NULL;
        eventHandling_ = false;

        callPendingFunctors();    //处理poll等待过程中发生的事件
    }
}

void EventLoop::stop()
{
    running_ = false;
    if (!isInLoopThread())
    {
        wakeupPoller();
    }
}

void EventLoop::updateChannel(Channel* channel)
{
    LOG_INFO("EventLoop[%0x]::updateChannel [%d]", this, channel->fd());
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (eventHandling_)
    {
        assert(currentActiveChannel_ == channel && "must be current channel!");
        assert(std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end() && "why");
    }
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
    return poller_->hasChannel(channel);
}

void EventLoop::runInLoop(const Functor& func)
{
    LOG_INFO("EventLoop[%0x]::runInLoop [%d][%0x]", this, isInLoopThread(), &func);
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
    LOG_INFO("EventLoop[%0x]::runInLoop [%d][%0x]", this, isInLoopThread(), &func);
    {
        MutexLocker lock(mutex_);
        pendingFunctors_.push_back(func);
    }

    if (!isInLoopThread() || !callingPendingFunctors_) // may be should wakeup at any time
    {
        wakeupPoller();
    }
}

void EventLoop::callPendingFunctors()
{
    std::vector<Functor> tmp_functors;
    callingPendingFunctors_ = true;
    {
        MutexLocker lock(mutex_);
        tmp_functors.swap(pendingFunctors_);
    }

    for (size_t i = 0; i < tmp_functors.size(); ++i)
    {
        tmp_functors[i]();
    }
    callingPendingFunctors_ = false;
}

void EventLoop::assertInLoopThread() const 
{
    if(!isInLoopThread())
    {
        LOG_ALERT("EventLoop::abortNotInLoopThread - EventLoop [%0x] was created in threadId_ [%d], " 
            "but current thread id = [%d].", this, currentThreadId_.pid(), this_thread::get_id().pid());
        assert("EventLoop::assertInLoopThread()" && "why???");
    }
}

void EventLoop::wakeupPoller()
{
    uint64_t value = 1;
    size_t n = SocketUtil::write(wakeupfd_, &value, sizeof(value));
    if (n != sizeof(value))
    {
        LOG_ERROR("EventLoop::wakeupPoller() wakeupfd_ write error[%d][%d]", n, errno);
    }
}

void EventLoop::handleRead()
{
    uint64_t value = 0;
    size_t n = SocketUtil::read(wakeupfd_, &value, sizeof(value));
    if (n != sizeof(value)) //always return 8 byte
    {
        LOG_ERROR("EventLoop::handleRead() wakeupfd_ read error[%d][%d]", n, errno);
    }
}

NAMESPACE_ZL_NET_END