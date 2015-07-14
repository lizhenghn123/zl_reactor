#include "base/Logger.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include "base/Timestamp.h"
#include "base/FileUtil.h"
#include "base/LogFile.h"
#include "thread/Thread.h"
NAMESPACE_ZL_BASE_START

#define MAX_PRIORITY_NAME_LENGTH   (9)
#define MAX_LOG_ENTRY_SIZE         (4096)                /* 每次log输出的最大大小(B) */

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

namespace detail
{
    void defaultConsoleOutput(const char* msg, size_t len)
    {
        size_t n = fwrite(msg, 1, len, stdout);
        (void)n;
        //fflush(stdout);
    }

    void defaultFlush()
    {
        fflush(stdout);
    }
}

/*static*/ ZLogOutput            Logger::mode_        = ZL_LOG_OUTPUT_DEFAULT;
/*static*/ ZLogPriority          Logger::priority_    = ZL_LOG_PRIO_INFO;
/*static*/ ZLogHeader            Logger::header_      = ZL_LOG_HEADER_DEFAULT;
/*static*/ ZLogMasking           Logger::masking_     = ZL_LOG_MASKING_COMPLETE;
/*static*/ log_ext_handler_f     Logger::ext_handler_ = NULL;

Logger::Logger()
{
}

Logger::~Logger()
{
}

bool Logger::init(ZLogOutput mode, ZLogHeader header, ZLogPriority priority, ZLogMasking mask)
{
    header_ = header;
    priority_ = priority;
    masking_ = mask;
    setOutputMode(mode);
    return true;
}

log_ext_handler_f Logger::setLogHandler(log_ext_handler_f handler)
{
    log_ext_handler_f old = ext_handler_;
    ext_handler_ = handler;
    return old;
}

ZLogPriority Logger::setLogPriority(ZLogPriority prio)
{
    ZLogPriority old = priority_;
    priority_ = prio;
    return old;
}

void Logger::setConsoleOutput(bool optval/* = true*/)
{
    if (optval)
        mode_ = (ZLogOutput)(mode_ | ZL_LOG_OUTPUT_CONSOLE);
    else
        mode_ = (ZLogOutput)(mode_ & (~ZL_LOG_OUTPUT_CONSOLE));
}

void Logger::setOutputMode(ZLogOutput mode)
{
    mode_ = mode;
    //if (mode_ & ZL_LOG_OUTPUT_FILE)
    //{
    //    if (!logFile_)
    //        logFile_ = new LogFile;
    //    ext_handler_ = std::bind(&LogFile::dumpLog, logFile_);
    //}
    //else if (mode_ & ZL_LOG_OUTPUT_ASYNC_FILE)
    //{
    //    //ext_handler_ = ;
    //}
}

void Logger::disableLog()
{
    priority_ = ZL_LOG_PRIO_DISABLE;
}

ZLogPriority Logger::logPriority()
{
    return priority_;
}

bool Logger::log(const char *file, int line, ZLogPriority priority, const char *format, ...)
{
    if (priority > priority_ || ((mode_ & ZL_LOG_OUTPUT_CONSOLE) == ZL_LOG_OUTPUT_NONE && !ext_handler_))
    {
        return true;
    }

    va_list arg_ptr;
    va_start(arg_ptr, format);
    char log_entry[MAX_LOG_ENTRY_SIZE];
    size_t max_size = MAX_LOG_ENTRY_SIZE - 2;
    size_t offset = 0;

    zl::base::Timestamp time = zl::base::Timestamp::now();
    struct tm result = time.getTm();

    if (header_ & ZL_LOG_HEADER_DATE)
    {
        offset += ZL_SNPRINTF(log_entry + offset, max_size - offset, "%4d-%02d-%02d ",
            result.tm_year + 1900, result.tm_mon + 1, result.tm_mday);
    }
    if (header_ & ZL_LOG_HEADER_TIME)
    {
        offset += ZL_SNPRINTF(log_entry + offset, max_size - offset, "%02d:%02d:%02d:%06d ",
            result.tm_hour, result.tm_min, result.tm_sec, int(time.microSeconds() % ZL_USEC_PER_SEC));
    }
    if (header_ & ZL_LOG_HEADER_PRIORITY)
    {
        ::memcpy(log_entry + offset, priority_snames[priority], MAX_PRIORITY_NAME_LENGTH);
        offset += MAX_PRIORITY_NAME_LENGTH;
    }
    if (header_ & ZL_LOG_HEADER_MARK)
    {
        offset += ZL_SNPRINTF(log_entry + offset, max_size - offset, "%s:%04d ", file, line);
    }
    if (header_ & ZL_LOG_HEADER_THREAD)
    {
        offset += ZL_SNPRINTF(log_entry + offset, max_size - offset, "[%d] ", thread::this_thread::tid());
    }

    int size = vsnprintf(log_entry + offset, max_size - offset, format, arg_ptr);
    offset += (size > 0 ? size : 0);

    log_entry[offset++] = '\n';
    log_entry[offset] = '\0';
    va_end(arg_ptr);

    if ((mode_ & ZL_LOG_OUTPUT_CONSOLE) == ZL_LOG_OUTPUT_CONSOLE)
    {
        detail::defaultConsoleOutput(log_entry, offset);
    }

    if (ext_handler_)
    {
        ext_handler_(log_entry, offset);
    }

    return true;
}

NAMESPACE_ZL_BASE_END
