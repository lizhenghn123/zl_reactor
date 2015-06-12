#include "base/ZLog.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include "base/Timestamp.h"
#include "base/FileUtil.h"

NAMESPACE_ZL_BASE_START

#define MAX_PRIORITY_NAME_LENGTH (9)
#define MAX_LOG_ENTRY_SIZE        (4096)                /* 每次log输出的最大大小(B) */

#ifdef OS_WINDOWS
#define ZL_SNPRINTF  _snprintf
#else
#define ZL_SNPRINTF  snprintf
#endif

class  ZLog;
class  ZLogFile;

static const char priority_snames[ZL_LOG_PRIO_COUNT][MAX_PRIORITY_NAME_LENGTH + 1] =
{
    ""        ,
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

namespace detail
{
    void defaultOutput(const char* msg, int len)
    {
        size_t n = fwrite(msg, 1, len, stdout);
        (void)n;
    }

    void defaultFlush()
    {
        fflush(stdout);
    }

    class GlobalZLog
    {
    public:
         GlobalZLog()
         {
              //zl_log_instance_create("log", zl::getBinaryName().c_str());
             zl_log_instance_create();
              assert(g_zlogger);
         }
         ~GlobalZLog()
         {
             zl_log_instance_destroy();
         }
    };
    static GlobalZLog __private_initialise_zlog__;

}

class ZLog
{
    friend bool zl_log(const char *file, int line, ZLogPriority priority, const char *format, ...);
    friend ZLogPriority zl_log_set_priority(ZLogPriority prio);
    friend void zl_log_console_output(bool optval);

public:
    ZLog()
    {
        mode_        = ZL_LOG_OUTPUT_DEFAULT;
        priority_    = ZL_LOG_PRIO_INFO;
        header_      = ZL_LOG_HEADER_DEFAULT;
        masking_     = ZL_LOG_MASKING_COMPLETE;
        ext_handler_ = NULL;

        setLogHandler(detail::defaultOutput);
    }

    ~ZLog()
    {
    }

    bool init(ZLogOutput mode = ZL_LOG_OUTPUT_DEFAULT,  ZLogHeader header = ZL_LOG_HEADER_DEFAULT,
              ZLogPriority priority = ZL_LOG_PRIO_INFO, ZLogMasking mask = ZL_LOG_MASKING_COMPLETE
              );

    void setLogHandler(zl_log_ext_handler_f handler)
    {
        ext_handler_ = handler;
    }

    bool zlog(const char *file, int line, ZLogPriority priority, const char *format, va_list arg_ptr);

public:
    ZLogOutput            mode_;
    ZLogPriority          priority_;
    ZLogHeader            header_;
    ZLogMasking           masking_;
    zl_log_ext_handler_f  ext_handler_;
private:
    ZLog(const ZLog&);
    ZLog& operator=(const ZLog&);
};

bool zl_log_instance_create(ZLogOutput mode/* = ZL_LOG_OUTPUT_DEFAULT*/,  ZLogHeader header/* = ZL_LOG_HEADER_DEFAULT*/,
                            ZLogPriority priority/* = ZL_LOG_PRIO_INFO*/, ZLogMasking mask/* = ZL_LOG_MASKING_COMPLETE*/
                            )
{
    if (g_zlogger)
    {
        zl_log_instance_destroy();
    }

    g_zlogger = new ZLog;

    return g_zlogger->init(mode, header, priority, mask);
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

void zl_log_disable_all()
{
    g_zlogger->priority_ = ZL_LOG_PRIO_DISABLE;
}

ZLogPriority zl_log_set_priority(ZLogPriority prio)
{
    ZLogPriority old = g_zlogger->priority_;
    g_zlogger->priority_ = prio;

    return old;
}

void zl_log_console_output(bool optval)
{
    if(optval)
        g_zlogger->mode_ = (ZLogOutput)(g_zlogger->mode_ | ZL_LOG_OUTPUT_CONSOLE);
    else
        g_zlogger->mode_ = (ZLogOutput)(g_zlogger->mode_ & (~ZL_LOG_OUTPUT_CONSOLE));
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
        char log_entry[MAX_LOG_ENTRY_SIZE];
        size_t max_size = MAX_LOG_ENTRY_SIZE - 2;
        size_t offset = 0;

        zl::base::Timestamp time = zl::base::Timestamp::now();
        struct tm *result = time.getTm();

        if (g_zlogger->header_ & ZL_LOG_HEADER_DATE)
        {
            offset += ZL_SNPRINTF(log_entry + offset, max_size - offset, "%4d-%02d-%02d ",
                result->tm_year + 1900, result->tm_mon + 1, result->tm_mday);
        }
        if (g_zlogger->header_ & ZL_LOG_HEADER_TIME)
        {
            offset += ZL_SNPRINTF(log_entry + offset, max_size - offset, "%02d:%02d:%02d:%06d ",
                result->tm_hour, result->tm_min, result->tm_sec, int(time.microSeconds() % ZL_USEC_PER_SEC));
        }
        if (g_zlogger->header_ & ZL_LOG_HEADER_PRIORITY)
        {
            memcpy(log_entry + offset, priority_snames[priority], MAX_PRIORITY_NAME_LENGTH);
            offset += MAX_PRIORITY_NAME_LENGTH;
        }
        if (g_zlogger->header_ & ZL_LOG_HEADER_MARK)
        {
            offset += ZL_SNPRINTF(log_entry + offset, max_size - offset, "%s:%04d ", file, line);
        }
        if (g_zlogger->header_ & ZL_LOG_HEADER_THREAD)
        {
            //offset += ZL_SNPRINTF(log_entry + offset, max_size - offset, "%05lu ", apt_thread_id_get());
        }

        //offset += vsprintf(log_entry + offset, format, arg_ptr); //vsprintf_s is not support in linux
        int size = vsnprintf(log_entry + offset, max_size - offset, format, arg_ptr);
        offset += (size > 0 ? size : 0);

        log_entry[offset++] = '\n';
        log_entry[offset] = '\0';
        va_end(arg_ptr);

        if ((g_zlogger->mode_ & ZL_LOG_OUTPUT_CONSOLE) == ZL_LOG_OUTPUT_CONSOLE)
        {
            ::fwrite(log_entry, offset, 1, stdout);
        }

        if (g_zlogger->ext_handler_)
        {
            g_zlogger->ext_handler_(log_entry, offset);
        }
        else
        {
            g_zlogger->zlog(file, line, priority, format, arg_ptr);
        }
    }
    return status;
}


bool ZLog::init(ZLogOutput mode/* = ZL_LOG_OUTPUT_DEFAULT*/,  ZLogHeader header/* = ZL_LOG_HEADER_DEFAULT*/,
                ZLogPriority priority/* = ZL_LOG_PRIO_INFO*/, ZLogMasking mask/* = ZL_LOG_MASKING_COMPLETE*/
                )
{
    mode_     = mode;
    header_   = header;
    priority_ = priority;
    masking_  = mask;

    return true;
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

    //offset += vsprintf(log_entry + offset, format, arg_ptr); //vsprintf_s is not support in linux
    int size = vsnprintf(log_entry + offset, MAX_LOG_ENTRY_SIZE - offset, format, arg_ptr);
    if(size > 0)
        offset += size;

    log_entry[offset++] = '\n';
    log_entry[offset] = '\0';
    if ((mode_ & ZL_LOG_OUTPUT_CONSOLE) == ZL_LOG_OUTPUT_CONSOLE)
    {
        ::fwrite(log_entry, offset, 1, stdout);
    }

    //if ((mode_ & ZL_LOG_OUTPUT_FILE) == ZL_LOG_OUTPUT_FILE && log_file_)
    //{
    //    log_file_->dumpLog(log_entry, offset);
    //}
    return true;
}

NAMESPACE_ZL_BASE_END
