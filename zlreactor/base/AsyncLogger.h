// ***********************************************************************
// Filename         : AsyncLogger.h
// Author           : LIZHENG
// Created          : 2015-06-15
// Description      : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_ASYNCLOGGER_H
#define ZL_ASYNCLOGGER_H
#include "Define.h"
#include <vector>
#include <memory>
namespace zl
{
namespace thread
{
    class Thread;
    class Mutex;
    class Condition;
    class CountDownLatch;
}
}
NAMESPACE_ZL_BASE_START

template <size_t SIZE>
class LogBuffer
{
public:
    LogBuffer()
    {
        current_ = 0;
    }
    //template <size_t SIZE>
    //LogBuffer(LogBuffer<SIZE> &buf)
    //{
    //    cout << ".....\n";
    //}
    //LogBuffer& operator=(const LogBuffer<SIZE>&)
    //{
    //    return *this;
    //}
    LogBuffer(const char* data, size_t len)
    {
        current_ = 0;
        append(data, len);
    }
    void append(const char* data, size_t len)
    {
        if (current_ + len < SIZE)
        {
            ::memcpy(data_ + current_, data, len);
            current_ += len;
        }
        else
        {
            ::memcpy(data_ + current_, data, SIZE - current_);
            current_ = SIZE;
        }
    }
    void reset()
    {
        current_ = 0;
    }
    size_t size() const
    {
        return current_;
    }
    size_t avail() const
    {
        return SIZE - current_;
    }
    const char* data() const
    {
        return data_;
    }
    std::string asString() const
    {
        return std::string(data_, current_);
    }
    void swap(LogBuffer &buf)
    {
        std::swap(data_, buf.data_);
        std::swap(current_, buf.current_);
    }
private:
    size_t  current_;
    char    data_[SIZE];
};

class AsyncLogger
{
public:
    explicit AsyncLogger(int flushInterval = 3);
    ~AsyncLogger();

    void start();
    void stop();
    void output(const char* data, size_t len);

private:
    void logThread();

private:
    typedef LogBuffer<4096 * 100>      Buffer;
    typedef std::unique_ptr<Buffer>    BufferPtr;

    bool                        isRunning_;
    thread::Mutex               *mutex_;
    thread::Condition           *condition_;
    thread::CountDownLatch      *latch_;
    thread::Thread              *thread_;

    int                         flushInterval_;     /// seconds
    size_t                      maxBufferSize_;
    size_t                      currentBufferSize_;
    BufferPtr                   currentBuffer_;
    BufferPtr                   remainBuffer_;
    std::vector<BufferPtr>      buffers_;
};

NAMESPACE_ZL_BASE_END
#endif /* ZL_ASYNCLOGGER_H */
