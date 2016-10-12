#include "zlreactor/net/Eventfd.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include "zlreactor/base/Logger.h"
#include "zlreactor/net/SocketUtil.h"
NAMESPACE_ZL_NET_START

EventfdHandler::EventfdHandler(unsigned int initval/* = 0 */, int flags/* = EFD_NONBLOCK | EFD_CLOEXEC */)
{
    eventfd_ = -1;
    createEventfd(initval, flags);
}

EventfdHandler::~EventfdHandler()
{
     if(eventfd_ != -1)
     {
         ::close(eventfd_);
         eventfd_ = -1;
     }
}

int EventfdHandler::createEventfd(unsigned int initval, int flags)
{
    int efd = ::eventfd(initval, flags);
    if (efd < 0)
    {
        LOG_ALERT("create eventfd failed in EventfdHandler::createEventfd()");
        return efd;
    }
    LOG_INFO("EventfdHandler::createEventfd [%d]", efd);

    eventfd_ = efd;
    return efd;
}

ssize_t EventfdHandler::write(uint64_t value/* = 1 */)
{
    ssize_t n = ::write(eventfd_, &value, sizeof(value));
    if (n != sizeof(value))  // just write one uint64_t
    {
        LOG_ERROR("EventfdHandler::write(): write error[%d][%d][%d]", eventfd_, n, errno);
    }
    return n;
}

ssize_t EventfdHandler::read(uint64_t *value/* = NULL*/)
{
    ssize_t n;
    if (value == NULL)
    {
        uint64_t tmp;
        n = ::read(eventfd_, &tmp, sizeof(uint64_t));
    }
    else
    {
        n = ::read(eventfd_, value, sizeof(uint64_t));
    }

    if (n != sizeof(value)) //always return 8 byte
    {
        LOG_ERROR("EventfdHandler::read(): read error[%d][%d][%d][%s]", eventfd_, n, errno, strerror(errno));
    }
    
    return n;
}

NAMESPACE_ZL_NET_END