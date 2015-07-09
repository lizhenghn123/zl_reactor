#include "base/LogFile.h"
#include "base/FileUtil.h"
NAMESPACE_ZL_BASE_START

LogFile::LogFile(const char *log_name/* = NULL*/, const char *log_dir/* = NULL*/, bool threadSafe/* = true*/, int flushInterval/* = 3*/,
                int flushCount/* = 1024*/, size_t max_file_size/* = MAX_LOG_FILE_SIZE*/, size_t max_file_count/* = MAX_LOG_FILE_COUNT*/, bool append/* = true*/)
    : flushInterval_(flushInterval)
    , flushCount_(flushCount)
    , maxFileSize_(max_file_size <= 0 ? MAX_LOG_FILE_SIZE : max_file_size)
    , maxFileCount_(max_file_count <= 0 ? MAX_LOG_FILE_COUNT : max_file_count)
    , mutex_(threadSafe ? new thread::Mutex : NULL)
{
    file_ = NULL;
    count_ = 0;
    lastFlush_ = 0;
    curSize_ = curFileIndex_ = 0;
    isThreadSafe_ = threadSafe;
    ::memset(logDir_, 0, MAX_FILE_PATH_LEN);
    ::memset(logFileName_, 0, MAX_FILE_PATH_LEN);
    ::memset(currLogFileName_, 0, MAX_FILE_PATH_LEN);

    init(log_dir, log_name, append);
    assert((isThreadSafe_ && mutex_) || (!isThreadSafe_ && !mutex_));
}

LogFile::~LogFile()
{
    if (file_ != NULL)
    {
        fclose(file_);
        file_ = NULL;
    }
    if (mutex_)
    {
        delete mutex_;
        mutex_ = NULL;
    }
}

void LogFile::setThreadSafe(bool optval)
{
    isThreadSafe_ = optval;
    if (isThreadSafe_ && !mutex_)
    {
        mutex_ = new thread::Mutex;
        assert(isThreadSafe_ && mutex_);
    }
}

void LogFile::init(const char *log_name, const char *log_dir, bool append)
{
    if (!log_name)
        log_name = zl::getBinaryName().c_str();
    if (!log_dir)
        log_dir = "log";

    ::memcpy(logFileName_, log_name, strlen(log_name) + 1);
    ::memcpy(logDir_, log_dir, strlen(log_dir) + 1);
    curFileIndex_ = 0;
    curSize_ = 0;

    if (!zl::isDirectory(logDir_))
        zl::createRecursionDir(log_dir);

    if (append)
    {
        /* iteratively find the last created file */
        while (curFileIndex_ < maxFileCount_)
        {
            const char *log_file_path = makeLogFilePath();
            if (!zl::isFileExist(log_file_path))
            {
                if (curFileIndex_ > 0)
                    curFileIndex_--;
                log_file_path = makeLogFilePath();
                curSize_ = zl::getFileSize(log_file_path);
                break;
            }
            curFileIndex_++;
        }

        /* if all the files have been created start rewriting from beginning */
        if (curFileIndex_ >= maxFileCount_)
        {
            curFileIndex_ = 0;
            curSize_ = 0;
            const char *log_file_path = makeLogFilePath();
            file_ = fopen(log_file_path, "wb"); /* truncate the first file to zero length */
            fclose(file_);
        }
    }

    /* 打开当前要输出的日志文件 */
    const char *log_file_path = makeLogFilePath();
    file_ = fopen(log_file_path, append == true ? "ab" : "wb");
}

void LogFile::dumpLog(const char *log_entry, size_t size)
{
    if (isThreadSafe_)
    {
        thread::LockGuard<thread::Mutex> lock(*mutex_);
        dumpLogWithHold(log_entry, size);
    }
    else
    {
        dumpLogWithHold(log_entry, size);
    }
}

void LogFile::dumpLogWithHold(const char *log_entry, size_t size)
{
    curSize_ += size;
    if (curSize_ > maxFileSize_)
    {
        ::fclose(file_);    // 关闭当前日志文件
        curFileIndex_++;    // 寻找下一日志文件
        curFileIndex_ %= maxFileCount_;

        const char *log_file_path = makeLogFilePath();
        file_ = ::fopen(log_file_path, "wb");
        if (!file_)
        {
            return;
        }
        curSize_ = size;
        count_ = 0;
    }

    ::fwrite(log_entry, 1, size, file_);
    count_ ++;
    if(count_ > flushCount_)
    {
        count_ = 0;
        time_t now = ::time(NULL);
        if(now - lastFlush_ > flushInterval_)
        {
            lastFlush_ = now;
            flush();
        }
    }
}

void LogFile::flush()
{
    ::fflush(file_);
    //::fwrite_unlocked(log_entry, 1, size, file_);
}

const char* LogFile::makeLogFilePath()
{
    ::memset(currLogFileName_, '\0', MAX_FILE_PATH_LEN);
    ZL_SNPRINTF(currLogFileName_, MAX_FILE_PATH_LEN, "%s/%s-%d.log", logDir_, logFileName_, curFileIndex_);
    return currLogFileName_;
}

NAMESPACE_ZL_BASE_END