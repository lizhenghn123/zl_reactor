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
	    zl::thread::LockGuard<zl::thread::Mutex> lock(mutex_);
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
        zl::thread::LockGuard<zl::thread::Mutex> lock(mutex_);
        addTimerWithHold(timer);
    }
}

void TimerQueue::deleteTimer(Timer * timer)
{
    if(checkTimer_)
        delTimes_.push_back(timer);
    else
    {
	    zl::thread::LockGuard<zl::thread::Mutex> lock(mutex_);
        deleteTimerWithHold(timer);
    }
}

void TimerQueue::addTimerWithHold(Timer * timer)
{
    timers_.insert(std::make_pair(timer->expires_at(), timer));
}

void TimerQueue::deleteTimerWithHold(Timer * timer)
{
    //timers_.erase(timer);
    std::pair<TimerList::iterator, TimerList::iterator> range = timers_.equal_range(timer->expires_at());
    for(TimerList::iterator start = range.first; start != range.second; ++start)
    {
        if(start->second == timer)
        {
            timers_.erase(start);
            break;
        }
    }
}

uint64_t howMuchTimeFromNow(Timestamp when)
{
    Timestamp now(Timestamp::now());
    return Timestamp::timediff(when, now) * 100000;
}

/*定时器延迟时间线程*/
void TimerQueue::processThread()
{
    static int DEFULT_INTERVAL = 10 * 1000;  //10ms
    timeval tm;

    while(running_)
    {
        tm.tv_sec = 0;
        tm.tv_usec = DEFULT_INTERVAL;

        //assert(timers_.begin()->first == timers_.begin()->second->expires_at());
        //uint64_t microseconds = howMuchTimeFromNow(timers_.begin()->first);
        //tm.tv_sec = microseconds / 1000000;
        //tm.tv_usec = microseconds % 1000000;
        ////printf("---[%ld][%ld][%ld]\n", microseconds, tm.tv_sec, tm.tv_usec);

        while(select(0, 0, 0, 0, &tm) < 0 && errno == EINTR);
        
        Timestamp now(Timestamp::now());
        //printf("[%s] select\n", now.toString().c_str());
        {
		    zl::thread::LockGuard<zl::thread::Mutex> lock(mutex_);
            checkTimer_ = true;
            for(TimerList::iterator iter = timers_.begin(); iter!=timers_.end(); )
            {
                Timer *timer = iter->second;
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
                addTimerWithHold(addTimes_[i]);
            addTimes_.clear();
            for(size_t i = 0; i < delTimes_.size(); ++i)
                deleteTimerWithHold(delTimes_[i]);
            delTimes_.clear();
        }
    }
}

NAMESPACE_ZL_NET_END
