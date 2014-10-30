#include "net/Channel.h"
#include <sstream>
#include <assert.h>
#include "net/Connection.h"
#include "net/EventLoop.h"
NAMESPACE_ZL_NET_START

//const int Channel::kNoneEvent = SOCKETEVENT_NONE;
//const int Channel::kReadEvent = SOCKETEVENT_READ;
//const int Channel::kWriteEvent = SOCKETEVENT_WRITE;
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, ZL_SOCKET fd__)
	: loop_(loop),
	fd_(fd__),
	events_(0),
	revents_(0),
	index_(-1),
	logHup_(true),
	tied_(false),
	eventHandling_(false),
	addedToLoop_(false)
{
}

Channel::~Channel()
{
	assert(!eventHandling_);
	assert(!addedToLoop_);
	//if (loop_->isInLoopThread())
	{
		assert(!loop_->hasChannel(this));
	}
}

void Channel::update()
{
	addedToLoop_ = true;
	//loop_->UpdateChannel(this);
}

void Channel::remove()
{
	assert(isNoneEvent());
	addedToLoop_ = false;
	loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
	//boost::shared_ptr<void> guard;
	//if (tied_)
	//{
	//	guard = tie_.lock();
	//	if (guard)
	//	{
	//		handleEventWithGuard(receiveTime);
	//	}
	//}
	//else
	{
		handleEventWithHold(receiveTime);
	}
}

void Channel::handleEventWithHold(Timestamp receiveTime)
{
	eventHandling_ = true;
	//LOG_TRACE << reventsToString();
	if ((revents_ & SOCKETEVENT_HUP) && !(revents_ & SOCKETEVENT_READ))
	{
		if (logHup_)
		{
			//LOG_WARN << "Channel::handle_event() POLLHUP";
		}
		if (closeCallback_) closeCallback_();
	}

	//if (revents_ & POLLNVAL)
	//{
	//	LOG_WARN << "Channel::handle_event() POLLNVAL";
	//}

	if (revents_ & (SOCKETEVENT_ERROR/* | POLLNVAL*/))
	{
		if (errorCallback_) errorCallback_();
	}
	if (revents_ & (SOCKETEVENT_READ/* | POLLPRI */| SOCKETEVENT_HUP))
	{
		if (readCallback_) readCallback_(receiveTime);
	}
	if (revents_ & SOCKETEVENT_WRITE)
	{
		if (writeCallback_) writeCallback_();
	}
	eventHandling_ = false;
}

std::string Channel::reventsToString() const
{
	std::ostringstream oss;
	oss << fd_ << ": ";
	if (revents_ & SOCKETEVENT_READ)
		oss << "IN ";
	//if (revents_ & POLLPRI)
	//	oss << "PRI ";
	if (revents_ & SOCKETEVENT_WRITE)
		oss << "OUT ";
	if (revents_ & SOCKETEVENT_HUP)
		oss << "HUP ";
	//if (revents_ & POLLRDHUP)
	//	oss << "RDHUP ";
	if (revents_ & SOCKETEVENT_ERROR)
		oss << "ERR ";
	//if (revents_ & POLLNVAL)
	//	oss << "NVAL ";

	return oss.str().c_str();
}

NAMESPACE_ZL_NET_END