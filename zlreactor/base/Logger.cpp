#include "base/Logger.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include "base/Timestamp.h"
#include "base/FileUtil.h"
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

#define ZL_USE_TERMINAL_COLOR     // 此宏表示在屏幕上输出日志时是否尝试使用带色彩

#ifdef ZL_USE_TERMINAL_COLOR

    enum ZLogColor
    {
        COLOR_NONE,
        COLOR_EMERGENCY,
        COLOR_ALERT,
        COLOR_CRITICAL,
        COLOR_ERROR,
        COLOR_WARNING,
        COLOR_NOTICE,
        COLOR_INFO,
        COLOR_DEBUG,

        COLOR_COUNT
    };

    // Returns true iff terminal supports using colors in output. see google-glog
    static bool isTerminalSupportsColor() 
    {
        bool term_supports_color = false;
    #ifdef OS_WINDOWS
        // on Windows TERM variable is usually not set, but the console does support colors.
        term_supports_color = true;
    #else
        // On non-Windows platforms, we rely on the TERM variable.
        const char* const term = getenv("TERM");
        if (term != NULL && term[0] != '\0')
        {
            term_supports_color =
                !strcmp(term, "xterm") ||
                !strcmp(term, "xterm-color") ||
                !strcmp(term, "xterm-256color") ||
                !strcmp(term, "screen") ||
                !strcmp(term, "linux") ||
                !strcmp(term, "cygwin");
        }
    #endif
        return term_supports_color;
    }
    
    const bool& terminalSupportsColor()
    {
        static bool terminal_supports_color = isTerminalSupportsColor();
        return terminal_supports_color;
    }

    static ZLogColor priorityToColor(ZLogPriority severity)
    {
        assert(severity >= 0 && severity < ZL_LOG_PRIO_COUNT);
        switch (severity)
        {
        case ZL_LOG_PRIO_EMERGENCY:     return COLOR_EMERGENCY;
        case ZL_LOG_PRIO_ALERT:         return COLOR_ALERT;
        case ZL_LOG_PRIO_CRITICAL:      return COLOR_CRITICAL;
        case ZL_LOG_PRIO_ERROR:         return COLOR_ERROR;
        case ZL_LOG_PRIO_WARNING:       return COLOR_WARNING;
        case ZL_LOG_PRIO_NOTICE:        return COLOR_NOTICE;
        case ZL_LOG_PRIO_INFO:          return COLOR_INFO;
        case ZL_LOG_PRIO_DEBUG:         return COLOR_NONE;
        default:                        assert(false); // should never get here.
        }
        return COLOR_NONE;
    }

    #ifdef OS_WINDOWS
    // Returns the character attribute for the given color.
    WORD getColorAttribute(ZLogColor color)
    {
        switch (color)
        {
        case COLOR_EMERGENCY:           return 4;           // red
        case COLOR_ALERT:               return 4;           // red
        case COLOR_CRITICAL:            return 6;           // yellow
        case COLOR_ERROR:               return 2;           // green
        case COLOR_WARNING:             return 5;           // purple
        case COLOR_NOTICE:              return 9;           // light blue
        case COLOR_INFO:                return 3;           // aqua
        case COLOR_DEBUG:               return 0;           // none
        default:                        return 0;
        }
        return 0;
    }
    #else
    // Returns the ANSI color code for the given color.
    const char* getAnsiColorCode(ZLogColor color)
    {
        switch (color)
        {
        case COLOR_EMERGENCY:       return "\033[0;32;31m";     // red
        case COLOR_ALERT:           return "\033[0;32;31m";     // red
        case COLOR_CRITICAL:        return "\033[0;32;33m";     // yellow
        case COLOR_ERROR:           return "\033[0;35m";        // purple
        case COLOR_WARNING:         return "\033[0;32m";        // green
        case COLOR_NOTICE:          return "\033[0;32;34m";     // blue
        case COLOR_INFO:            return "\033[1;34m";        // light blue
        case COLOR_DEBUG:           return "\033[m";            // none
        default:                    return "\033[m";
        }
        return NULL; // stop warning about return type.
    }
    #endif  // OS_WINDOWS

    //see google-glog
    static void coloredWriteToStderr(ZLogPriority severity, const char* message, size_t len)
    {
        const ZLogColor color = terminalSupportsColor() ? priorityToColor(severity) : COLOR_NONE;
        if (color == COLOR_NONE)
        {
            fwrite(message, len, 1, stderr);
            return;
        }

    #ifdef OS_WINDOWS
        const HANDLE stderr_handle = GetStdHandle(STD_ERROR_HANDLE);

        // Gets the current text color.
        CONSOLE_SCREEN_BUFFER_INFO buffer_info;
        GetConsoleScreenBufferInfo(stderr_handle, &buffer_info);
        const WORD old_color_attrs = buffer_info.wAttributes;

        // We need to flush the stream buffers into the console before each
        // SetConsoleTextAttribute call lest it affect the text that is already
        // printed but has not yet reached the console.
        fflush(stderr);
        SetConsoleTextAttribute(stderr_handle, getColorAttribute(color) | FOREGROUND_INTENSITY);
        fwrite(message, len, 1, stderr);
        fflush(stderr);        
        SetConsoleTextAttribute(stderr_handle, old_color_attrs);   // Restores the text color.
    #else
        fprintf(stderr, "%s", getAnsiColorCode(color));
        fwrite(message, 1, len, stderr);
        fprintf(stderr, "\033[m");  // Resets the terminal to default.
    #endif
    }
#endif  // ZL_USE_TERMINAL_COLOR

    void defaultConsoleOutput(ZLogPriority severity, const char* msg, size_t len)
    {
    #ifdef ZL_USE_TERMINAL_COLOR
        coloredWriteToStderr(severity, msg, len);
    #else
        (void)severity;
        size_t n = fwrite(msg, 1, len, stderr);
        (void)n;
        //fflush(stderr);
    #endif
    }

    void defaultFlush()
    {
        fflush(stderr);
    }
}

/*static*/ ZLogOutput            Logger::mode_        = ZL_LOG_OUTPUT_DEFAULT;
/*static*/ ZLogPriority          Logger::priority_    = ZL_LOG_PRIO_INFO;
/*static*/ ZLogHeader            Logger::header_      = ZL_LOG_HEADER_DEFAULT;
/*static*/ ZLogMasking           Logger::masking_     = ZL_LOG_MASKING_COMPLETE;
/*static*/ log_ext_handler_f     Logger::ext_handler_ /*= NULL*/;

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
        detail::defaultConsoleOutput(priority, log_entry, offset);
        //detail::coloredWriteToStderr(priority, log_entry, offset);
    }

    if (ext_handler_)
    {
        ext_handler_(log_entry, offset);
    }

    return true;
}

NAMESPACE_ZL_BASE_END
