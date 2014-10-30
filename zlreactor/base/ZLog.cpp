#include "base/ZLog.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include "thread/Mutex.h"
#include "base/Timestamp.h"
#include "base/FileUtil.h"
NAMESPACE_ZL_BASE_START

#define MAX_LOG_ENTRY_SIZE 4096      /* 每次log输出的最大大小 */
#define MAX_FILE_PATH_LEN  1024      /* 日志文件路径最大长度 */
#define MAX_PRIORITY_NAME_LENGTH 9

#ifdef OS_WINDOWS
#define ZL_SNPRINTF  _snprintf
#else
#define ZL_SNPRINTF  snprintf
#endif

class  ZLog;
class  ZLogFile;

static const char priority_snames[ZL_LOG_PRIO_COUNT][MAX_PRIORITY_NAME_LENGTH + 1] =
{
    "[EMERG]  ",
    "[ALERT]  ",
    "[CRITIC] ",
    "[ERROR]  ",
    "[WARN]   ",
    "[NOTICE] ",
    "[INFO]   ",
    "[DEBUG]  "
};

static ZLog *g_zlogger = NULL;

class ZLogFile
{
public:
    ZLogFile()
    {
        file_ = NULL;
        max_file_size_ = MAX_LOG_FILE_SIZE;
        max_file_count_ = MAX_LOG_FILE_COUNT;
        cur_size_ = cur_file_index_ = 0;
        append_ = true;
        ::memset(log_dir_, 0, MAX_FILE_PATH_LEN);
        ::memset(log_file_name_, 0, MAX_FILE_PATH_LEN);
        ::memset(curr_log_file_, 0, MAX_FILE_PATH_LEN);
    }
    ~ZLogFile()
    {
        if (file_ != NULL)
        {
            fclose(file_);
            file_ = NULL;
        }
    }
    bool init(const char *log_dir, const char *log_name, size_t max_file_size = MAX_LOG_FILE_SIZE,
              size_t max_file_count = MAX_LOG_FILE_COUNT, bool append = true);

    const char *makeLogFilePath();

    bool dumpLog(const char *log_entry, size_t size);

private:
    char               log_dir_[MAX_FILE_PATH_LEN];
    char               log_file_name_[MAX_FILE_PATH_LEN];
    char               curr_log_file_[MAX_FILE_PATH_LEN];
    FILE               *file_;
    size_t             max_file_size_;
    size_t             max_file_count_;
    size_t             cur_size_;
    size_t             cur_file_index_;
    bool               append_;
	zl::thread::Mutex  mutex_;
};

class ZLog
{
    friend bool zl_log(const char *file, int line, ZLogPriority priority, const char *format, ...);

public:
    ZLog()
    {
        log_file_    = new ZLogFile;
        mode_        = ZL_LOG_OUTPUT_DEFAULT;
        priority_    = ZL_LOG_PRIO_INFO;
        header_      = ZL_LOG_HEADER_DEFAULT;
        masking_     = ZL_LOG_MASKING_COMPLETE;
        ext_handler_ = NULL;
    }

    ~ZLog()
    {
        delete log_file_;
        log_file_ = NULL;
    }

    bool init(const char *log_dir, const char *log_name,
              size_t max_file_size = MAX_LOG_FILE_SIZE, size_t max_file_count = MAX_LOG_FILE_COUNT,
              ZLogOutput mode = ZL_LOG_OUTPUT_DEFAULT,  ZLogHeader header = ZL_LOG_HEADER_DEFAULT,
              ZLogPriority priority = ZL_LOG_PRIO_INFO, ZLogMasking mask = ZL_LOG_MASKING_COMPLETE,
              bool append = true);

    void setLogHandler(zl_log_ext_handler_f handler)
    {
        ext_handler_ = handler;
    }

    bool zlog(const char *file, int line, ZLogPriority priority, const char *format, va_list arg_ptr);

private:
    ZLogOutput            mode_;
    ZLogPriority          priority_;
    ZLogHeader            header_;
    ZLogMasking           masking_;
    ZLogFile              *log_file_;
    zl_log_ext_handler_f  ext_handler_;
private:
    ZLog(const ZLog&);
    ZLog& operator=(const ZLog&);
};

bool zl_log_instance_create(const char *log_dir, const char *log_name,
                            size_t max_file_size/* = MAX_LOG_FILE_SIZE*/, size_t max_file_count/*= MAX_LOG_FILE_COUNT*/,
                            ZLogOutput mode/* = ZL_LOG_OUTPUT_DEFAULT*/,  ZLogHeader header/* = ZL_LOG_HEADER_DEFAULT*/,
                            ZLogPriority priority/* = ZL_LOG_PRIO_INFO*/, ZLogMasking mask/* = ZL_LOG_MASKING_COMPLETE*/,
                            bool append/* = true*/)
{
    if (g_zlogger)
    {
        return true;
    }
    else if (!log_dir || !log_name)
    {
        return false;
    }

    g_zlogger = new ZLog;

    return g_zlogger->init(log_dir, log_name, max_file_size, max_file_count, mode, header, priority, mask, append);
}

bool zl_log_instance_destroy()
{
    if (!g_zlogger)
    {
        return true;
    }

    delete g_zlogger;
    g_zlogger = NULL;

    return true;
}

void zl_log_set_handler(zl_log_ext_handler_f handler)
{
    g_zlogger->setLogHandler(handler);
}

bool zl_log(const char *file, int line, ZLogPriority priority, const char *format, ...)
{
    if (!g_zlogger)
    {
        return false;
    }

    bool status = true;
    if (priority <= g_zlogger->priority_)
    {
        va_list arg_ptr;
        va_start(arg_ptr, format);
        if (g_zlogger->ext_handler_)
        {
            status = g_zlogger->ext_handler_(file, line, NULL, priority, format, arg_ptr);
        }
        else
        {
            status = g_zlogger->zlog(file, line, priority, format, arg_ptr);
        }
        va_end(arg_ptr);
    }
    return status;
}


bool ZLog::init(const char *log_dir, const char *log_name,
                size_t max_file_size/* = MAX_LOG_FILE_SIZE*/, size_t max_file_count/*= MAX_LOG_FILE_COUNT*/,
                ZLogOutput mode/* = ZL_LOG_OUTPUT_DEFAULT*/,  ZLogHeader header/* = ZL_LOG_HEADER_DEFAULT*/,
                ZLogPriority priority/* = ZL_LOG_PRIO_INFO*/, ZLogMasking mask/* = ZL_LOG_MASKING_COMPLETE*/,
                bool append/* = true*/)
{
    mode_     = mode;
    header_   = header;
    priority_ = priority;
    masking_  = mask;

    return log_file_->init(log_dir, log_name, max_file_size, max_file_count, append);
}

bool ZLog::zlog(const char *file, int line, ZLogPriority priority, const char *format, va_list arg_ptr)
{
    char log_entry[MAX_LOG_ENTRY_SIZE];
    size_t max_size = MAX_LOG_ENTRY_SIZE - 2;
    size_t offset = 0;

	zl::base::Timestamp time = zl::base::Timestamp::now();
	struct tm *result = time.getTm();

    if (header_ & ZL_LOG_HEADER_DATE)
    {
        offset += ZL_SNPRINTF(log_entry + offset, max_size - offset, "%4d-%02d-%02d ",
                              result->tm_year + 1900, result->tm_mon + 1, result->tm_mday);
    }
    if (header_ & ZL_LOG_HEADER_TIME)
    {
        offset += ZL_SNPRINTF(log_entry + offset, max_size - offset, "%02d:%02d:%02d:%06d ",
			result->tm_hour, result->tm_min, result->tm_sec, int(time.microSeconds() % ZL_USEC_PER_SEC));
    }
    if (header_ & ZL_LOG_HEADER_MARK)
    {
        offset += ZL_SNPRINTF(log_entry + offset, max_size - offset, "%s:%03d ", file, line);
    }
    if (header_ & ZL_LOG_HEADER_THREAD)
    {
        //offset += ZL_SNPRINTF(log_entry + offset, max_size - offset, "%05lu ", apt_thread_id_get());
    }
    if (header_ & ZL_LOG_HEADER_PRIORITY)
    {
        memcpy(log_entry + offset, priority_snames[priority], MAX_PRIORITY_NAME_LENGTH);
        offset += MAX_PRIORITY_NAME_LENGTH;
    }

    offset += vsprintf(log_entry + offset, format, arg_ptr); //vsprintf_s is not support in linux
    log_entry[offset++] = '\n';
    log_entry[offset] = '\0';
    if ((mode_ & ZL_LOG_OUTPUT_CONSOLE) == ZL_LOG_OUTPUT_CONSOLE)
    {
        ::fwrite(log_entry, offset, 1, stdout);
    }

    if ((mode_ & ZL_LOG_OUTPUT_FILE) == ZL_LOG_OUTPUT_FILE && log_file_)
    {
        log_file_->dumpLog(log_entry, offset);
    }
    return true;
}


bool ZLogFile::init(const char *log_dir, const char *file_name, size_t max_file_size/* = MAX_LOG_FILE_SIZE*/,
                    size_t max_file_count/* = MAX_LOG_FILE_COUNT*/, bool append/* = true*/)
{
	::memcpy(log_dir_, log_dir, strlen(log_dir) + 1);
    ::memcpy(log_file_name_, file_name, strlen(file_name) + 1);
    cur_file_index_ = 0;
    cur_size_ = 0;
    append_ = append;

    max_file_size_ = (max_file_size <= 0 ? MAX_LOG_FILE_SIZE : max_file_size);
    max_file_count_ = (max_file_count <= 0 ? MAX_LOG_FILE_COUNT : max_file_count);

    if (!zl::isDirectory(log_dir_)) 
        zl::createRecursionDir(log_dir);

    if (append_ == true)
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

    return (file_ == NULL ? false : true);
}

const char *ZLogFile::makeLogFilePath()
{
    ::memset(curr_log_file_, '\0', MAX_FILE_PATH_LEN);
	ZL_SNPRINTF(curr_log_file_, MAX_FILE_PATH_LEN, "%s/%s-%d.log", log_dir_, log_file_name_, (int)cur_file_index_);
    return curr_log_file_;
}

bool ZLogFile::dumpLog(const char *log_entry, size_t size)
{
	zl::thread::MutexLocker lock(mutex_);

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
            return false;
        }
        cur_size_ = size;
    }

    ::fwrite(log_entry, 1, size, file_);
    ::fflush(file_);

    return true;
}

NAMESPACE_ZL_BASE_END
