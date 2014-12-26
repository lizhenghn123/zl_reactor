#include "net/Channel.h"
#include <sstream>
#include <assert.h>
#include "base/ZLog.h"
#include "net/EventLoop.h"
NAMESPACE_ZL_NET_START

const int Channel::kEventNone  = 0;
const int Channel::kEventRead  = POLLIN | POLLPRI;
const int Channel::kEventWrite = POLLOUT;

Channel::Channel(EventLoop* loop, ZL_SOCKET fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0)
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
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        LOG_INFO("Channel::handleEventWithHold closeCallback, fd[%d]", fd_); 
        if (closeCallback_)
            closeCallback_();
    }

    if (revents_ & POLLNVAL)
    { 
        LOG_WARN("Channel::handle_event() POLLNVAL, fd[%d]", fd_);
    }

    if (revents_ & (POLLERR | POLLNVAL))
    {
        LOG_INFO("Channel::handleEventWithHold closeCallback, fd[%d]", fd_);
        if (errorCallback_)
            errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (readCallback_) 
            readCallback_(receiveTime);
    }
    if (revents_ & POLLOUT)
    {
        if (writeCallback_)
            writeCallback_();
    }
}

std::string Channel::reventsToString() const
{
    std::ostringstream oss;
    oss << fd_ << ": ";
    if (revents_ & POLLIN)
        oss << "IN ";
    if (revents_ & POLLPRI)
        oss << "PRI ";
    if (revents_ & POLLOUT)
        oss << "OUT ";
    if (revents_ & POLLHUP)
        oss << "HUP ";
    if (revents_ & POLLRDHUP)
        oss << "RDHUP ";
    if (revents_ & POLLERR)
        oss << "ERR ";
    if (revents_ & POLLNVAL)
        oss << "NVAL ";

    return oss.str().c_str();
}

NAMESPACE_ZL_NET_END