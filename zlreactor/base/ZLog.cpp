#include "base/ZLog.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include "base/Timestamp.h"
#include "base/FileUtil.h"

NAMESPACE_ZL_BASE_START

#define MAX_PRIORITY_NAME_LENGTH   (9)
#define MAX_LOG_ENTRY_SIZE          (4096)                /* 每次log输出的最大大小(B) */

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

static Logger *g_zlogger = NULL;

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
             g_zlogger = new Logger;
             assert(g_zlogger);
         }
         ~GlobalZLog()
         {
             delete g_zlogger;
         }
    };
    static GlobalZLog __private_initialise_zlog__;

}

/*static*/ ZLogOutput            Logger::mode_        = ZL_LOG_OUTPUT_DEFAULT;
/*static*/ ZLogPriority          Logger::priority_    = ZL_LOG_PRIO_INFO;
/*static*/ ZLogHeader            Logger::header_      = ZL_LOG_HEADER_DEFAULT;
/*static*/ ZLogMasking           Logger::masking_     = ZL_LOG_MASKING_COMPLETE;
/*static*/ log_ext_handler_f     Logger::ext_handler_ = detail::defaultOutput;

Logger::Logger()
{
}

Logger::~Logger()
{
}

bool Logger::init(ZLogOutput mode, ZLogHeader header, ZLogPriority priority, ZLogMasking mask)
{
    mode_ = mode;
    header_ = header;
    priority_ = priority;
    masking_ = mask;
    return true;
}

void Logger::setLogHandler(log_ext_handler_f handler)
{
    g_zlogger->ext_handler_ = handler;
}

ZLogPriority Logger::setLogPriority(ZLogPriority prio)
{
    ZLogPriority old = g_zlogger->priority_;
    g_zlogger->priority_ = prio;
    return old;
}

void Logger::setConsoleOutput(bool optval/* = true*/)
{
    if (optval)
        g_zlogger->mode_ = (ZLogOutput)(g_zlogger->mode_ | ZL_LOG_OUTPUT_CONSOLE);
    else
        g_zlogger->mode_ = (ZLogOutput)(g_zlogger->mode_ & (~ZL_LOG_OUTPUT_CONSOLE));
}

void Logger::disableLog()
{
    g_zlogger->priority_ = ZL_LOG_PRIO_DISABLE;
}

ZLogPriority Logger::logPriority()
{
    return priority_;
}

bool Logger::log(const char *file, int line, ZLogPriority priority, const char *format, ...)
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

        int size = vsnprintf(log_entry + offset, max_size - offset, format, arg_ptr);
        offset += (size > 0 ? size : 0);

        log_entry[offset++] = '\n';
        log_entry[offset] = '\0';
        va_end(arg_ptr);

        if ((g_zlogger->mode_ & ZL_LOG_OUTPUT_CONSOLE) == ZL_LOG_OUTPUT_CONSOLE)
        {
            ::fwrite(log_entry, offset, 1, stdout);
        }
        else if (g_zlogger->ext_handler_)
        {
            g_zlogger->ext_handler_(log_entry, offset);
        }

    }
    return status;
}

NAMESPACE_ZL_BASE_END
