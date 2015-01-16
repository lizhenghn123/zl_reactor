#include "net/EventLoop.h"
#include <assert.h>
#include <sys/eventfd.h>     // for eventfd
#include <signal.h>          // for ::signal
#include "net/Channel.h"
#include "net/poller/Poller.h"
#include "base/Timestamp.h"
#include "base/ZLog.h"
#include "net/TimerQueue.h"
using namespace zl::thread;
NAMESPACE_ZL_NET_START

namespace
{
    // 如果往已关闭的client socket上继续write时，服务器进程会收到SIGPIPE信号而终止,
    // 这里简单忽略该信号
    class IgnoreSigPipe
    {
    public:
        IgnoreSigPipe()
        {
            ::signal(SIGPIPE, SIG_IGN);
        }
    }_dont_use_this_class_;

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

    timerQueue_ = new TimerQueue(this);
}

EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupfd_);
    Safe_Delete(wakeupChannel_);
    Safe_Delete(poller_);
}

void EventLoop::loop()
{
    running_ = true;

    Timestamp now;
    while (running_)
    {
        activeChannels_.clear();

        int timeoutMs = 0;
        now = Timestamp::now();
        Timestamp nextExpired = timerQueue_->getNearestExpiration();
        if(nextExpired.valid())
        {
            
            double seconds = Timestamp::timediff(nextExpired, now);
            LOG_INFO("nextExpired.valid() [%s][%s][%lf]", nextExpired.toString().c_str(), now.toString().c_str(), seconds);
            if(seconds <= 0)
                timeoutMs = 0;
            else
                timeoutMs = seconds * 1000;
        }
        else
        {
        #if defined(POLL_WAIT_INDEFINITE)
            timeoutMs = -1;
        #else
            timeoutMs = 0;
        #endif
        }

        now = poller_->poll_once(timeoutMs, activeChannels_);
        //LOG_INFO("EventLoop::loop [%s][%d]", now.toString().c_str(), activeChannels_.size());

        eventHandling_ = true;
        for (ChannelList::iterator it = activeChannels_.begin(); it != activeChannels_.end(); ++it)
        {
            currentActiveChannel_ = *it;
            currentActiveChannel_->handleEvent(now);
        }
        currentActiveChannel_ = NULL;
        eventHandling_ = false;

        timerQueue_->runTimer(now);

        callPendingFunctors();    //处理poll等待过程中发生的事件
    }
}

void EventLoop::quit()
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
        LockGuard<Mutex> lock(mutex_);
        pendingFunctors_.push_back(func);
    }

    if (!isInLoopThread() || !callingPendingFunctors_) // may be should wakeup at any time
    {
        wakeupPoller();
    }
}

TimerId EventLoop::addTimer(const TimerCallback& cb, const Timestamp& when)
{
    return timerQueue_->addTimer(cb, when, 0);
}

TimerId EventLoop::addTimer(const TimerCallback& cb, double delaySeconds, bool repeat)
{
    Timestamp when(Timestamp::now());
    when += delaySeconds;
    return timerQueue_->addTimer(cb, when, repeat ? delaySeconds : 0);
}

void EventLoop::cancelTimer(TimerId id)
{
     timerQueue_->cancelTimer(id);
}

void EventLoop::callPendingFunctors()
{
    std::vector<Functor> tmp_functors;
    callingPendingFunctors_ = true;
    {
        LockGuard<Mutex> lock(mutex_);
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
            "but current thread id = [%d].", this, currentThreadId_.tid(), this_thread::get_id().tid());
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
