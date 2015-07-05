#include "AsyncLogger.h"
#include "thread/Thread.h"
#include "thread/Mutex.h"
#include "thread/Condition.h"
#include "thread/CountDownLatch.h"
#include "base/Logger.h"
#include "base/LogFile.h"
NAMESPACE_ZL_BASE_START

AsyncLogger::AsyncLogger(int flushInterval/* = 3*/)
    : isRunning_(false),
      mutex_(new thread::Mutex),
      condition_(new thread::Condition(*mutex_)),
      latch_(new thread::CountDownLatch(1)),
      thread_(new thread::Thread(std::bind(&AsyncLogger::logThread, this), "AsyncLogger")),
      flushInterval_(flushInterval),
      currentBuffer_(new Buffer),
      remainBuffer_(new Buffer)
{
    //latch_->wait();
}

AsyncLogger::~AsyncLogger()
{
    if (isRunning_)
    {
        stop();
    }
    thread_->join();
    delete mutex_;
    delete condition_;
    delete latch_;
    delete thread_;
}

void AsyncLogger::start()
{
    LOG_SET_LOGHANDLER(std::bind(&AsyncLogger::output, this, std::placeholders::_1, std::placeholders::_2));
    latch_->wait();
}

void AsyncLogger::stop()
{
    isRunning_ = false;
    condition_->notify_all();
}

void AsyncLogger::output(const char* data, size_t len)
{
    thread::LockGuard<thread::Mutex> lock(*mutex_);
    if (currentBuffer_->avail() > len)
    {
        currentBuffer_->append(data, len);
    }
    else
    {
        buffers_.push_back(std::move(currentBuffer_));
        if (remainBuffer_)
        {
            currentBuffer_ = std::move(remainBuffer_);
        }
        else
        {
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_->append(data, len);
        condition_->notify_all();
    }
}

void AsyncLogger::logThread()
{
    isRunning_ = true;
    LogFile logfile(NULL, NULL, false, 3);
    std::vector<BufferPtr> writeBuffers;
    latch_->countDown();

    while(isRunning_)
    {
        {
            thread::LockGuard<thread::Mutex> lock(*mutex_);
            if (buffers_.empty())
            {
                condition_->timed_wait(flushInterval_ * 1000);
            }
            //if (!isRunning_)
            //{
            //    break;
            //}
            buffers_.push_back(std::move(currentBuffer_));
            if (remainBuffer_)
            {
                currentBuffer_ = std::move(remainBuffer_);
            }
            else
            {
                currentBuffer_.reset(new Buffer);
            }
            writeBuffers.swap(buffers_);
        }

        assert(!writeBuffers.empty());
        for (size_t i = 0; i < writeBuffers.size(); ++i)
        {
            logfile.dumpLog(writeBuffers[i]->data(), writeBuffers[i]->size());
        }

        {
            thread::LockGuard<thread::Mutex> lock(*mutex_);
            if (!remainBuffer_)
            {
                remainBuffer_ = std::move(writeBuffers[0]);
                remainBuffer_.reset();
            }
        }

        writeBuffers.clear();
        logfile.flush();
    }
    logfile.flush();
}

NAMESPACE_ZL_BASE_END
