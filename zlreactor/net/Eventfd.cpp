#include "net/Eventfd.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include "base/ZLog.h"
#include "net/SocketUtil.h"
NAMESPACE_ZL_NET_START

EventfdHandler::EventfdHandler()
{
    eventfd_ = -1;
}

EventfdHandler::~EventfdHandler()
{
     if(eventfd_ != -1)
     {
         ::close(eventfd_);
         eventfd_ = -1;
     }
}

int EventfdHandler::createEventfd()
{
    int efd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (efd < 0)
    {
        LOG_ALERT("create eventfd failed in EventfdHandler::createEventfd()");
        return efd;
    }
    LOG_INFO("EventfdHandler::createEventfd [%d]", efd);

    eventfd_ = efd;
    return efd;
}

ssize_t EventfdHandler::write(const void *data, size_t len)
{
    return SocketUtil::write(eventfd_, data, len);
}

ssize_t EventfdHandler::read(void *buf, size_t size)
{
	return SocketUtil::read(eventfd_, buf, size);
}

NAMESPACE_ZL_NET_END