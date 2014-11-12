#include "TriggerTimer.h"
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>

TriggerTimer::TriggerTimer(unsigned int interval,void (*func)(TriggerTimer *,void *),void *args,TimerType type/* = TIMER_ONCE*/)
        : timer_interval_(interval), timer_duration_(interval),
        timer_state_(TIMER_IDLE), timer_type_(type),
        callback_(func), args_(args)
{
    assert(callback_);
    assert(args_);
    trigger_sum_ = 0;
}

TriggerTimer::~TriggerTimer()
{
    if(timer_state_==TIMER_ALIVE)
        Stop();
}

void TriggerTimer::Start()
{
    TriggerTimerManager::instance()->AddTimer(this);
}

void TriggerTimer::Stop()
{
    TriggerTimerManager::instance()->DeleteTimer(this);
}

void TriggerTimer::Reset(unsigned int interval)
{
    TriggerTimerManager::instance()->DeleteTimer(this);
    timer_duration_ = timer_interval_= interval;
    TriggerTimerManager::instance()->AddTimer(this);
}

bool TriggerTimer::Trigger()
{
    ++trigger_sum_;
    if(callback_)
    {
        callback_(this,args_);
    }
    return true;
}


TriggerTimerManager* TriggerTimerManager::m_instance = NULL;
pthread_mutex_t TriggerTimerManager::m_mutex = PTHREAD_MUTEX_INITIALIZER;

TriggerTimerManager* TriggerTimerManager::instance()
{
    if(TriggerTimerManager::m_instance==NULL)
    {
        pthread_mutex_lock(&m_mutex);
        if(m_instance==NULL)
        {
            m_instance=new TriggerTimerManager();
        }
        pthread_mutex_unlock(&m_mutex);
    }
    return m_instance;
}

TriggerTimerManager::TriggerTimerManager()
{
    running_ = false;
    timers_.clear();
}

void TriggerTimerManager:: Start()
{
    if(!running_)
    {
        running_ = true;
        pthread_t pid;
        pthread_create(&pid, 0, ProcessThread, this);
    }
}

void TriggerTimerManager:: Stop()
{
    running_ = false;
}

void TriggerTimerManager::AddTimer(TriggerTimer * timer)
{
    pthread_mutex_lock(&m_mutex);
    AddTimer_(timer);
    pthread_mutex_unlock(&m_mutex);
}

void TriggerTimerManager::DeleteTimer(TriggerTimer * timer)
{
    pthread_mutex_lock(&m_mutex);
    DeleteTimer_(timer);
    pthread_mutex_unlock(&m_mutex);
}
   
void TriggerTimerManager::AddTimer_(TriggerTimer * timer)
{
    timers_.push_back(timer);
}

void TriggerTimerManager::DeleteTimer_(TriggerTimer * timer)
{
    timers_.remove(timer);
}

/*定时器延迟时间线程*/
void* TriggerTimerManager::ProcessThread(void * arg)
{
    pthread_detach(pthread_self());

    TriggerTimerManager *manage=(TriggerTimerManager *)arg;

    struct timeval start,end;
    unsigned int delay;
    
    struct timeval tm;
    gettimeofday(&end,0);
    
    const static int DEFULT_INTERVAL = 1;
    while(manage->running_)
    {
        tm.tv_sec=0;
        tm.tv_usec=DEFULT_INTERVAL*1000;
        start.tv_sec=end.tv_sec;
        start.tv_usec=end.tv_usec;
        
        //这里可以改为将延迟时间取list中最小的那个间隔，保证每次延迟时间到都有回调
        //或者使用multimap保存，每次找到最小间隔
        while(select(0,0,0,0,&tm)<0&&errno==EINTR);
        
        gettimeofday(&end,0);
        delay=(end.tv_sec-start.tv_sec)*1000+(end.tv_usec-start.tv_usec)/1000;
        
        pthread_mutex_lock(&manage->m_mutex);
        for(std::list<TriggerTimer*>::iterator iter = manage->timers_.begin(); iter!=manage->timers_.end(); )
        {
            TriggerTimer *timer = *iter;
            timer->timer_duration_ < delay ? timer->timer_duration_ = 0 : timer->timer_duration_ -= delay;
            if(timer->timer_duration_==0)
            {
                timer->Trigger();

                if(timer->timer_type_==TriggerTimer::TIMER_ONCE) /* 一次型的，超时则移除，并重置状态 */
                {
                    //manage->DeleteTimer_(timer);
                    iter = manage->timers_.erase(iter);
                    timer->timer_state_=TriggerTimer::TIMER_TIMEOUT;
                }
                else if(timer->timer_type_==TriggerTimer::TIMER_CIRCLE) /* 循环型的，重新计时 */
                {
                    timer->timer_duration_ = timer->timer_interval_;
                    ++iter;
                }
            }
            else
            {
                ++iter;
            }
        }
        pthread_mutex_unlock(&manage->m_mutex);
    } // while
    
    return NULL;
}




