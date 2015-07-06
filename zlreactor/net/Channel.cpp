#include "net/Channel.h"
#include <sstream>
#include <assert.h>
#include "base/Logger.h"
#include "net/EventLoop.h"
NAMESPACE_ZL_NET_START

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop)
    , fd_(fd)
    , events_(0)
    , revents_(0)
{
}

Channel::~Channel()
{
    if (loop_->isInLoopThread())
    {
        assert(!loop_->hasChannel(this));
    }
}

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::remove()
{
    assert(isNoneEvent());
    loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
    handleEventWithHold(receiveTime);
}

void Channel::handleEventWithHold(Timestamp receiveTime)
{
    if ((revents_ & FDEVENT_HUP) && !(revents_ & FDEVENT_IN))
    {
        LOG_INFO("Channel::handleEventWithHold closeCallback, fd[%d]", fd_); 
        if (closeCallback_)
            closeCallback_();
    }

    if (revents_ & FDEVENT_NVAL)
    { 
        LOG_WARN("Channel::handle_event() POLLNVAL, fd[%d]", fd_);
    }

    if (revents_ & (FDEVENT_ERR | FDEVENT_NVAL))
    {
        LOG_INFO("Channel::handleEventWithHold closeCallback, fd[%d]", fd_);
        if (errorCallback_)
            errorCallback_();
    }
    if (revents_ & kEventRead)
    {
        if (readCallback_) 
            readCallback_(receiveTime);
    }
    if (revents_ & FDEVENT_OUT)
    {
        if (writeCallback_)
            writeCallback_();
    }
}

std::string Channel::reventsToString() const
{
    std::ostringstream oss;
    oss << fd_ << ": ";
    if (revents_ & FDEVENT_IN)
        oss << "IN ";
    if (revents_ & FDEVENT_PRI)
        oss << "PRI ";
    if (revents_ & FDEVENT_OUT)
        oss << "OUT ";
    if (revents_ & FDEVENT_HUP)
        oss << "HUP ";
    if (revents_ & FDEVENT_RDHUP)
        oss << "RDHUP ";
    if (revents_ & FDEVENT_ERR)
        oss << "ERR ";
    if (revents_ & FDEVENT_NVAL)
        oss << "NVAL ";

    return oss.str().c_str();
}

NAMESPACE_ZL_NET_END
