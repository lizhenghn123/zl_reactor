#include "AsyncLogger.h"
#include "thread/Thread.h"
#include "thread/Mutex.h"
#include "thread/Condition.h"
#include "thread/CountDownLatch.h"
NAMESPACE_ZL_BASE_START

AsyncLogger::AsyncLogger(size_t maxBufferSize/* = 4096*/, size_t maxIntevalSecond/* = 3*/)
    : isRunning_(false),
      mutex_(new thread::Mutex),
      condition_(new thread::Condition(*mutex_)),
      latch_(new thread::CountDownLatch(1)),
      thread_(new thread::Thread(std::bind(&AsyncLogger::logThread, this), "AsyncLogger"))
{
    latch_->wait();
}

AsyncLogger::~AsyncLogger()
{
    delete mutex_;
    delete condition_;
    delete latch_;
    thread_->join();
    delete thread_;
}

void AsyncLogger::start()
{
    //latch_->wait();
}

void AsyncLogger::stop()
{
    isRunning_ = false;
    condition_->notify_all();
}

void AsyncLogger::output(const char* data, size_t len)
{
    if (currentBufferSize_ + len < maxBufferSize_)
    {
        currentBuffer_.append(data, len);
    }
    else
    {
        thread::LockGuard<thread::Mutex> lock(*mutex_);
        remainBuffer_.swap(currentBuffer_);
        currentBuffer_.append(data, len);
    }
}


void AsyncLogger::logThread()
{
    isRunning_ = true;
    latch_->countDown();
    while(isRunning_)
    {
        {
            thread::LockGuard<thread::Mutex> lock(*mutex_);
        }
    }
}

NAMESPACE_ZL_BASE_END
