#include "net/Timerfd.h"
#include "base/ZLog.h"
#include "net/SocketUtil.h"
NAMESPACE_ZL_NET_START

 //创建一个定时器描述符
int createTimerfd(int clockid/* = CLOCK_MONOTONIC*/, int flags/* = TFD_NONBLOCK | TFD_CLOEXEC*/)
{
    int timerfd = ::timerfd_create(clockid, flags);

    return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when)
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

//设置新的超时时间
void resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(oldValue));
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret)
    {
        LOG_ERROR("resetTimerfd [%d] [%d] [%s]", timerfd, ret, expiration.toString().c_str());
    }
}

//从时间文件描述符获得当前有多少个定时器超时
uint64_t readTimerfd(int timerfd, Timestamp now)
{
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
    LOG_DEBUG("readTimerfd [%d] [%d] [%s]", timerfd, howmany, now.toString().c_str());
    if (n != sizeof(howmany))
    {
        LOG_ERROR("readTimerfd [%d] [%d] [%s]", timerfd, howmany, now.toString().c_str());
    }
    return howmany;
}

NAMESPACE_ZL_NET_END
