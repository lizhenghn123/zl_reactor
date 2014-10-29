#include "thread/Timer.h"
#include "thread/TimerQueue.h"
#include <assert.h>
NAMESPACE_ZL_THREAD_START

Timer::Timer(unsigned int interval,void (*func)(Timer *,void *),void *args,TimerType type/* = TIMER_ONCE*/)
    : timer_interval_(interval), timer_duration_(interval),
    timer_state_(TIMER_IDLE), timer_type_(type),
    callback_(func), args_(args)
{
    assert(callback_);
    assert(args_);
    trigger_sum_ = 0;
}

Timer::~Timer()
{
    if(timer_state_==TIMER_ALIVE)
        stop();
}

void Timer::start()
{
    TimerQueue::instance()->addTimer(this);
}

void Timer::stop()
{
    TimerQueue::instance()->deleteTimer(this);
}

void Timer::reset(unsigned int interval)
{
    TimerQueue::instance()->deleteTimer(this);
    timer_duration_ = timer_interval_= interval;
    TimerQueue::instance()->addTimer(this);
}

bool Timer::trigger()
{
    ++trigger_sum_;
    if(callback_)
    {
        callback_(this,args_);
    }
    return true;
}

NAMESPACE_ZL_THREAD_END