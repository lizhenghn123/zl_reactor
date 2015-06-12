// ***********************************************************************
// Filename         : LogFile.h
// Author           : LIZHENG
// Created          : 2015-06-12
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2015-06-12
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

class ZLogFile
{
public:
    ZLogFile(const char *log_name = NULL, const char *log_dir = NULL, bool threadSafe = true, size_t max_file_size = MAX_LOG_FILE_SIZE,
        size_t max_file_count = MAX_LOG_FILE_COUNT, bool append = true);

    ~ZLogFile();

    const char *makeLogFilePath();

    void dumpLog(const char *log_entry, size_t size);

    void setThreadSafe(bool safe) { isThreadSafe_ = safe; }

private:
    void init(const char *log_name, const char *log_dir, size_t max_file_size, size_t max_file_count, bool append);
    void dumpLogWithHold(const char *log_entry, size_t size);

private:
    char               log_dir_[MAX_FILE_PATH_LEN];
    char               log_file_name_[MAX_FILE_PATH_LEN];
    char               curr_log_file_[MAX_FILE_PATH_LEN];
    FILE               *file_;
    size_t             max_file_size_;
    size_t             max_file_count_;
    size_t             cur_size_;
    size_t             cur_file_index_;
    bool               isThreadSafe_;
    zl::thread::Mutex  *mutex_;
};

NAMESPACE_ZL_BASE_END
#endif /* ZL_LOGFILE_H */