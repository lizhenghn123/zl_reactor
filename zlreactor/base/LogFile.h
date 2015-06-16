// ***********************************************************************
// Filename         : LogFile.h
// Author           : LIZHENG
// Created          : 2015-06-12
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-06-13
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_LOGFILE_H
#define ZL_LOGFILE_H
#include "Define.h"
#include "thread/Mutex.h"
NAMESPACE_ZL_BASE_START

#define MAX_LOG_FILE_SIZE  (100 * 1024 * 1024)   /** 默认每个日志文件大小（MB）*/
#define MAX_LOG_FILE_COUNT (10)                  /** 默认循环日志文件数量 */
#define MAX_FILE_PATH_LEN  (1024)                /* 日志文件路径最大长度 */

class LogFile
{
public:
    LogFile(const char *log_name = NULL, const char *log_dir = NULL, bool threadSafe = true, int flushInterval = 3, // second
          int flushCount = 1024, size_t max_file_size = MAX_LOG_FILE_SIZE, size_t max_file_count = MAX_LOG_FILE_COUNT, bool append = true);

    ~LogFile();

    void setThreadSafe(bool safe);

    void dumpLog(const char *log_entry, size_t size);

private:
    void init(const char *log_name, const char *log_dir, bool append);
    void dumpLogWithHold(const char *log_entry, size_t size);
    const char *makeLogFilePath();
    void flush();

private:
    char               logDir_[MAX_FILE_PATH_LEN];
    char               logFileName_[MAX_FILE_PATH_LEN];
    char               currLogFileName_[MAX_FILE_PATH_LEN];
    const int          flushInterval_;   /// seconds
    const int          flushCount_;
    const size_t       maxFileSize_;
    const int          maxFileCount_;

    FILE               *file_;
    int                count_;
    size_t             curSize_;
    int                curFileIndex_;
    bool               isThreadSafe_;
    time_t             lastFlush_;
    zl::thread::Mutex  *mutex_;
};

NAMESPACE_ZL_BASE_END
#endif /* ZL_LOGFILE_H */
