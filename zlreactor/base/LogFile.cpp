#include "base/LogFile.h"
#include "base/FileUtil.h"
NAMESPACE_ZL_BASE_START

ZLogFile::ZLogFile(const char *log_name/* = NULL*/, const char *log_dir/* = NULL*/, bool threadSafe/* = true*/, 
                    size_t max_file_size/* = MAX_LOG_FILE_SIZE*/, size_t max_file_count/* = MAX_LOG_FILE_COUNT*/, bool append/* = true*/)
    : mutex_(threadSafe ? new thread::Mutex : NULL)
{
    assert((threadSafe && mutex_) || !threadSafe);
    file_ = NULL;
    max_file_size_ = MAX_LOG_FILE_SIZE;
    max_file_count_ = MAX_LOG_FILE_COUNT;
    cur_size_ = cur_file_index_ = 0;
    isThreadSafe_ = true;
    ::memset(log_dir_, 0, MAX_FILE_PATH_LEN);
    ::memset(log_file_name_, 0, MAX_FILE_PATH_LEN);
    ::memset(curr_log_file_, 0, MAX_FILE_PATH_LEN);

    init(log_dir, log_name, max_file_size, max_file_count, append);
}

ZLogFile::~ZLogFile()
{
    if (file_ != NULL)
    {
        fclose(file_);
        file_ = NULL;
    }
}

void ZLogFile::init(const char *log_name, const char *log_dir, size_t max_file_size, size_t max_file_count, bool append)
{
    if (!log_name)
        log_name = zl::getBinaryName().c_str();
    if (!log_dir)
        log_dir = "log";

    ::memcpy(log_file_name_, log_name, strlen(log_name) + 1);
    ::memcpy(log_dir_, log_dir, strlen(log_dir) + 1);
    cur_file_index_ = 0;
    cur_size_ = 0;

    max_file_size_ = (max_file_size <= 0 ? MAX_LOG_FILE_SIZE : max_file_size);
    max_file_count_ = (max_file_count <= 0 ? MAX_LOG_FILE_COUNT : max_file_count);

    if (!zl::isDirectory(log_dir_))
        zl::createRecursionDir(log_dir);

    if (append)
    {
        /* iteratively find the last created file */
        while (cur_file_index_ < max_file_count_)
        {
            const char *log_file_path = makeLogFilePath();
            if (!zl::isFileExist(log_file_path))
            {
                if (cur_file_index_ > 0)
                    cur_file_index_--;
                log_file_path = makeLogFilePath();
                cur_size_ = zl::getFileSize(log_file_path);
                break;
            }
            cur_file_index_++;
        }

        /* if all the files have been created start rewriting from beginning */
        if (cur_file_index_ >= max_file_count_)
        {
            cur_file_index_ = 0;
            cur_size_ = 0;
            const char *log_file_path = makeLogFilePath();
            file_ = fopen(log_file_path, "wb"); /* truncate the first file to zero length */
            fclose(file_);
        }
    }

    /* 打开当前要输出的日志文件 */
    const char *log_file_path = makeLogFilePath();
    file_ = fopen(log_file_path, append == true ? "ab" : "wb");
}

const char* ZLogFile::makeLogFilePath()
{
    ::memset(curr_log_file_, '\0', MAX_FILE_PATH_LEN);
    ZL_SNPRINTF(curr_log_file_, MAX_FILE_PATH_LEN, "%s/%s-%d.log", log_dir_, log_file_name_, (int)cur_file_index_);
    return curr_log_file_;
}

void ZLogFile::dumpLog(const char *log_entry, size_t size)
{
    if (mutex_)
    {
        thread::LockGuard<thread::Mutex> lock(*mutex_);
        dumpLogWithHold(log_entry, size);
    }
    else
    {
        dumpLogWithHold(log_entry, size);
    }
}

void ZLogFile::dumpLogWithHold(const char *log_entry, size_t size)
{
    cur_size_ += size;
    if (cur_size_ > max_file_size_)
    {
        ::fclose(file_);    // 关闭当前日志文件
        cur_file_index_++;  // 寻找下一日志文件
        cur_file_index_ %= max_file_count_;

        const char *log_file_path = makeLogFilePath();
        file_ = ::fopen(log_file_path, "wb");
        if (!file_)
        {
            return;
        }
        cur_size_ = size;
    }

    ::fwrite(log_entry, 1, size, file_);
    ::fflush(file_);
}

NAMESPACE_ZL_BASE_END