#include "net/EventLoopThreadPool.h"
#include "net/EventLoop.h"
#include "thread/Thread.h"
#include "thread/CountDownLatch.h"
#include "base/StringUtil.h"
#include "base/Logger.h"
using zl::thread::Thread;
NAMESPACE_ZL_NET_START

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
    : baseLoop_(baseLoop)
    , started_(false)
    , numThreads_(0)
    , next_(0)
    , latch_(NULL)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    std::for_each(loops_.begin(), loops_.end(), std::bind(&EventLoop::quit, std::placeholders::_1));
    std::for_each(threads_.begin(), threads_.end(), std::bind(&Thread::join, std::placeholders::_1));

    assert(loops_.size() == threads_.size());
    for(size_t i = 0; i < loops_.size(); ++i)
    {
        Safe_Delete(threads_[i]);
    }
}

void EventLoopThreadPool::setMultiReactorThreads(int numThreads)
{
    ZL_ASSERT(numThreads >=0)(numThreads);
    if(numThreads < 0)
        numThreads = zl::thread::Thread::hardware_concurrency();
    numThreads_ = numThreads;
    latch_ = new zl::thread::CountDownLatch(numThreads_);
}

void EventLoopThreadPool::start()
{
    if(latch_ == NULL || numThreads_ < 0)
        setMultiReactorThreads(0);

    assert(!started_);
    started_ = true;
    baseLoop_->assertInLoopThread();

    for (int i = 0; i < numThreads_; ++i)
    {
        std::string thrd_name = "eventloop_";
        thrd_name += zl::base::toStr(i);
        Thread *thread = new Thread(std::bind(&EventLoopThreadPool::runLoop, this), thrd_name);
        threads_.push_back(thread);
    }
    latch_->wait();
    LOG_INFO("EventLoopThreadPool[%0x]::started [%ld][%d]", this, zl::thread::this_thread::get_id().tid(), numThreads_);
}

void EventLoopThreadPool::runLoop()
{
    EventLoop this_loop;

    {
        zl::thread::LockGuard<zl::thread::Mutex> lock(mutex_);
        loops_.push_back(&this_loop);
    }
    //zl::thread::this_thread::sleep_for(zl::thread::chrono::seconds(2));
    latch_->countDown();
    LOG_INFO("EventLoopThreadPool countDown [%0x]::runInLoop [%ld]", this, zl::thread::this_thread::get_id().tid());
    this_loop.loop();
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    assert(started_);
    baseLoop_->assertInLoopThread();

    EventLoop* loop = baseLoop_;

    if (!loops_.empty())     // round-robin
    {
        loop = loops_[next_];

        if (++next_ >= loops_.size())
        {
            next_ = 0;
        }
    }

    return loop;
}

NAMESPACE_ZL_NET_END
