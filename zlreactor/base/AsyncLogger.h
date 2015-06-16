// ***********************************************************************
// Filename         : AsyncLogger.h
// Author           : LIZHENG
// Created          : 2015-06-15
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-06-16
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_ASYNCLOGGER_H
#define ZL_ASYNCLOGGER_H
#include "Define.h"
#include <vector>
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

class LogBuffer
{
public:
    LogBuffer()
    {
        
    }
    LogBuffer(const char* data, size_t len)
    {
        data_.resize(len);
        ::memcpy(&*data_.begin(), data, len);
    }
    void append(const char* data, size_t len)
    {
        ::memcpy(&*data_.begin(), data, len);
    }
    size_t size()
    {
        return data_.size();
    }
    void swap(LogBuffer &buf)
    {
        std::swap(data_, buf.data_);
    }
    const char* data()
    {
        return data_.data();
    }
private:
    std::vector<char> data_;
};

class AsyncLogger
{
public:
    AsyncLogger(size_t maxBufferSize = 4096, size_t maxIntevalSecond = 3);
    ~AsyncLogger();

    void start();
    void stop();
    void output(const char* data, size_t len);

private:
    void logThread();

private:
    bool                        isRunning_;
    thread::Mutex               *mutex_;
    thread::Condition           *condition_;
    thread::CountDownLatch      *latch_;
    thread::Thread              *thread_;

    size_t                      maxBufferSize_;
    size_t                      currentBufferSize_;
    LogBuffer                   currentBuffer_;
    LogBuffer                   remainBuffer_;
};

NAMESPACE_ZL_BASE_END
#endif /* ZL_ASYNCLOGGER_H */
