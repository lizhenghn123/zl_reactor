#include "net/Timerfd.h"
#include "base/Logger.h"
#include "net/SocketUtil.h"
NAMESPACE_ZL_NET_START

TimerfdHandler::TimerfdHandler(int clockid/* = CLOCK_MONOTONIC*/, int flags/* = TFD_NONBLOCK | TFD_CLOEXEC*/)
    : timerfd_(-1)
{
    timerfd_ = ::timerfd_create(clockid, flags);  //创建一个定时器描述符
    if(timerfd_ < 0)
    {
        LOG_ERROR("TimerfdHandler create timerfd failure [%d] [%s]", errno, strerror(errno));
    }
}

TimerfdHandler::~TimerfdHandler()
{
    if (timerfd_ > 0)
    {
        ::close(timerfd_);
    }
}

static struct timespec howMuchTimeFromNow(Timestamp when)
{
    int64_t microseconds = when.microSeconds() - Timestamp::now().microSeconds();
    if (microseconds < 100)
    {
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec  = static_cast<time_t>(microseconds / ZL_USEC_PER_SEC);
    ts.tv_nsec = static_cast<long>((microseconds % ZL_USEC_PER_SEC) * 1000);
    return ts;
}

void TimerfdHandler::resetTimerfd(Timestamp expiration, int interval_us/* = 0*/)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(oldValue));

    newValue.it_value = howMuchTimeFromNow(expiration);
    if(interval_us > 0)
    {
        newValue.it_interval.tv_sec = interval_us / 1000000;
        newValue.it_interval.tv_nsec = interval_us % 1000000;
    }

    int ret = ::timerfd_settime(timerfd_, 0, &newValue, &oldValue);
    if (ret)
    {
        LOG_ERROR("resetTimerfd [%d] [%d] [%s]", timerfd_, ret, strerror(errno));
    }
}

//设置新的超时时间
void TimerfdHandler::resetTimerfd(uint64_t next_expire_us, int interval_us/* = 0*/)
{
    Timestamp expiration(Timestamp::now().microSeconds() + next_expire_us);
    resetTimerfd(expiration, interval_us);
}

//从时间文件描述符获得当前有多少个定时器超时
uint64_t TimerfdHandler::read(uint64_t *howmany)
{
    ssize_t n = ::read(timerfd_, howmany, sizeof(uint64_t));
    LOG_DEBUG("readTimerfd [%d] [%d]", timerfd_, *howmany);
    if (n != sizeof(uint64_t))
    {
        LOG_ERROR("readTimerfd [%d] [%d] [%s]", timerfd_, *howmany);
    }
    return n;
}

void TimerfdHandler::stop()
{
    struct itimerspec newValue;
    bzero(&newValue, sizeof(newValue));
    ::timerfd_settime(timerfd_, 0, &newValue, NULL);
}

NAMESPACE_ZL_NET_END
