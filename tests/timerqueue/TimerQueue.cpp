#include "TimerQueue.h"
#include "Timer.h"
#include "base/StopWatch.h"
#include <sys/timerfd.h>
NAMESPACE_ZL_NET_START

TimerQueue::TimerQueue() 
    : checkTimer_(false), thread_(std::bind(&TimerQueue::processThread, this))
{
    running_ = true;
    timers_.clear();
}

TimerQueue::~TimerQueue()
{
    running_ = false;
    {
        zl::thread::MutexLocker lock(m_mutex);
        timers_.clear();
    }
    thread_.join();
}

void TimerQueue::stop()
{
    running_ = false;
}

void TimerQueue::addTimer(Timer * timer)
{
    if(checkTimer_)
        addTimes_.push_back(timer);
    else
    {
        zl::thread::MutexLocker lock(m_mutex);
        addTimerWithHold(timer);
    }
}

void TimerQueue::deleteTimer(Timer * timer)
{
    if(checkTimer_)
        delTimes_.push_back(timer);
    else
    {
        zl::thread::MutexLocker lock(m_mutex);
        deleteTimerWithHold(timer);
    }
}

void TimerQueue::addTimerWithHold(Timer * timer)
{
    timers_.insert(timer);
}

void TimerQueue::deleteTimerWithHold(Timer * timer)
{
    timers_.erase(timer);
}

/*定时器延迟时间线程*/
void TimerQueue::processThread()
{
    timeval tm;

    const static int DEFULT_INTERVAL = 10 * 1000;  //10ms
    while(running_)
    {
        tm.tv_sec = 0;
        tm.tv_usec = DEFULT_INTERVAL;

        while(select(0, 0, 0, 0, &tm) < 0 && errno == EINTR);
        
        Timestamp now(Timestamp::now());
        {
            zl::thread::MutexLocker lock(m_mutex);
            checkTimer_ = true;
            for(std::set<Timer*>::iterator iter = timers_.begin(); iter!=timers_.end(); )
            {
                Timer *timer = *iter;
                if(timer->expires_at() < now)
                {
                    //iter = timers_.erase(iter);
                    timers_.erase(iter++);
                    timer->trigger();
                }
                else
                {
                    ++iter;
                }
            }
            checkTimer_ = false;
            for(size_t i = 0; i < addTimes_.size(); ++i)
                timers_.insert(addTimes_[i]);
            addTimes_.clear();
            for(size_t i = 0; i < delTimes_.size(); ++i)
                timers_.erase(delTimes_[i]);
            delTimes_.clear();
        }
    }
}

NAMESPACE_ZL_NET_END
