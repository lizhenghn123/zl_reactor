#include "Timer.h"
#include "TimerQueue.h"
#include <assert.h>
NAMESPACE_ZL_NET_START

Timer::Timer(TimerQueue *tqueue) 
    : timerQueue_(tqueue), callback_(NULL), when_()
{

}

Timer::Timer(TimerQueue *tqueue, const Timestamp& when)
    : timerQueue_(tqueue), callback_(NULL), when_(when)
{

}

Timer::Timer(TimerQueue *tqueue, size_t millsec)
    : timerQueue_(tqueue), callback_(NULL), when_(Timestamp::now() + millsec / 1000.0)
{

}

Timer::~Timer()
{   
    if(callback_ != NULL)
        timerQueue_->deleteTimer(this);
}


void Timer::wait()
{
	//Sleep(when_.millSeconds() + when_.seconds() * 1000); //¼´¿É
    if(when_.microSeconds() == 0)
        return ;
    Timestamp now = Timestamp::now();

    int64_t milliseconds = static_cast<int64_t>(Timestamp::timediff(when_, now) * 1000);
    zl::thread::this_thread::sleep_for(zl::thread::chrono::milliseconds(milliseconds));
}

void Timer::async_wait(TimerCallBack callback)
{
    callback_ = callback;
    timerQueue_->addTimer(this);
}

void Timer::trigger()
{
     if(callback_)
     {
         callback_();
         //when_ = Timestamp();
     }
}

Timestamp Timer::expires_at(const Timestamp& expiry_time)
{   
    Timestamp old = when_;
    when_ = expiry_time;
    return old;
}

Timestamp Timer::expires_at() const 
{ 
    return when_;
}

Timestamp Timer::expires_from_now(size_t millsec_time)
{
    Timestamp old = when_;
    when_ = Timestamp::now() + millsec_time / 1000.0;
    return old;
}

size_t Timer::expires_from_now() const
{
     Timestamp now = Timestamp::now();
     return Timestamp::timediff(when_, now);
}

size_t Timer::cancel()
{
    timerQueue_->deleteTimer(this);
    return Timestamp::timediff(when_, Timestamp::now());
}

NAMESPACE_ZL_NET_END
