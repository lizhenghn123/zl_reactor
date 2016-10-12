// ***********************************************************************
// Filename         : Logger.h
// Author           : LIZHENG
// Created          : 2014-07-13
// Description      : 日志类,可设置日志级别\输出格式\输出类型\重定向输出
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_ZLOG_H
#define ZL_ZLOG_H
#include "zlreactor/Define.h"
#include <stdarg.h>
#include <stdlib.h>
NAMESPACE_ZL_BASE_START

#define ZL_LOG_MARK  __FILE__,__LINE__  /** 输出格式：文件+行号 */
#define ZL_PTR_FMT   "<0x%x>"           /** 输出格式：指针地址值 */
#define ZL_STR_FMT   "<%s>"             /** 输出格式：字符串标识 */

/** Priority of log messages ordered from highest priority to lowest (rfc3164) */
enum ZLogPriority
{
    ZL_LOG_PRIO_DISABLE,     /**< disable all log */
    ZL_LOG_PRIO_EMERGENCY,   /**< system is unusable */
    ZL_LOG_PRIO_ALERT,       /**< action must be taken immediately */
    ZL_LOG_PRIO_CRITICAL,    /**< critical condition */
    ZL_LOG_PRIO_ERROR,       /**< error condition */
    ZL_LOG_PRIO_WARNING,     /**< warning condition */
    ZL_LOG_PRIO_NOTICE,      /**< normal, but significant condition */
    ZL_LOG_PRIO_INFO,        /**< informational message */
    ZL_LOG_PRIO_DEBUG,       /**< debug-level message */

    ZL_LOG_PRIO_COUNT        /**< number of priorities */
};

/** Header (format) of log messages */
enum ZLogHeader
{
    ZL_LOG_HEADER_NONE     = 0x00,    /**< disable optional headers output */
    ZL_LOG_HEADER_DATE     = 0x01,    /**< enable date output */
    ZL_LOG_HEADER_TIME     = 0x02,    /**< enable time output */
    ZL_LOG_HEADER_PRIORITY = 0x04,    /**< enable priority name output */
    ZL_LOG_HEADER_MARK     = 0x08,    /**< enable file:line mark output */
    ZL_LOG_HEADER_THREAD   = 0x10,    /**< enable thread identifier output */
    ZL_LOG_HEADER_DEFAULT  = ZL_LOG_HEADER_DATE | ZL_LOG_HEADER_TIME | ZL_LOG_HEADER_PRIORITY | ZL_LOG_HEADER_THREAD
};

/** Mode of log output */
enum ZLogOutput
{
    ZL_LOG_OUTPUT_NONE       = 0x00,     /**< disable logging */
    ZL_LOG_OUTPUT_CONSOLE    = 0x01,     /**< enable console output */
//    ZL_LOG_OUTPUT_FILE       = 0x02,     /**< enable log file output */
//    ZL_LOG_OUTPUT_ASYNC_FILE = 0x04,     /**< enable log file(async) output */
    ZL_LOG_OUTPUT_DEFAULT    = ZL_LOG_OUTPUT_CONSOLE/* | ZL_LOG_OUTPUT_FILE*/
};

/** Masking mode of private data */
enum ZLogMasking
{
    ZL_LOG_MASKING_NONE      = 0x00,  /**< log everything as is */
    ZL_LOG_MASKING_COMPLETE  = 0x01,  /**< mask private data completely */
    ZL_LOG_MASKING_ENCRYPTED = 0x02   /**< encrypt private data */
};

class LogFile;

/** Prototype of extended log handler function */
//typedef void(*log_ext_handler_f)(const char* msg, size_t len);
typedef std::function<void(const char* msg, size_t len)> log_ext_handler_f;

class Logger
{
public:
    Logger();
    ~Logger();

public:
    static bool              init(ZLogOutput mode = ZL_LOG_OUTPUT_DEFAULT, ZLogHeader header = ZL_LOG_HEADER_DEFAULT,
                                  ZLogPriority priority = ZL_LOG_PRIO_INFO, ZLogMasking mask = ZL_LOG_MASKING_COMPLETE);

    static log_ext_handler_f setLogHandler(log_ext_handler_f handler);
    static ZLogPriority      setLogPriority(ZLogPriority prio);
    static void              setOutputMode(ZLogOutput mode);
    static void              setConsoleOutput(bool optval = true);
    static void              disableLog();
    static ZLogPriority      logPriority();

    static bool              log(const char *file, int line, ZLogPriority priority, const char *format, ...);

private:
    static ZLogOutput        mode_;          /// 只设定是否在屏幕上输出
    static ZLogPriority      priority_;
    static ZLogHeader        header_;
    static ZLogMasking       masking_;
    static log_ext_handler_f ext_handler_;   /// 重定向输出，注意不应该是屏幕输出，默认一直屏幕输出, 此回调不应该是阻塞的！
    //static LogFile*          logFile_;
private:
    Logger(const Logger&);
    Logger& operator=(const Logger&);
};

#define LOG_SET_DEBUG_MODE            LOG_SET_PRIORITY(zl::base::ZL_LOG_PRIO_DEBUG)
#define LOG_SET_LOGHANDLER(handler)   zl::base::Logger::setLogHandler(handler)
#define LOG_SET_PRIORITY(priority)    zl::base::Logger::setLogPriority((zl::base::ZLogPriority)priority)
#define LOG_CONSOLE_OUTPUT(optval)    zl::base::Logger::setConsoleOutput(optval)
#define LOG_DISABLE_ALL               zl::base::Logger::disableLog()

#define LOG_DEBUG(s, ...)             zl::base::Logger::log(ZL_LOG_MARK, zl::base::ZL_LOG_PRIO_DEBUG,     s, ##__VA_ARGS__)
#define LOG_INFO(s, ...)              zl::base::Logger::log(ZL_LOG_MARK, zl::base::ZL_LOG_PRIO_INFO,      s, ##__VA_ARGS__)
#define LOG_NOTICE(s, ...)            zl::base::Logger::log(ZL_LOG_MARK, zl::base::ZL_LOG_PRIO_NOTICE,    s, ##__VA_ARGS__)
#define LOG_WARN(s, ...)              zl::base::Logger::log(ZL_LOG_MARK, zl::base::ZL_LOG_PRIO_WARNING,   s, ##__VA_ARGS__)
#define LOG_ERROR(s, ...)             zl::base::Logger::log(ZL_LOG_MARK, zl::base::ZL_LOG_PRIO_ERROR,     s, ##__VA_ARGS__)
#define LOG_CRITICA(s, ...)           zl::base::Logger::log(ZL_LOG_MARK, zl::base::ZL_LOG_PRIO_CRITICAL,  s, ##__VA_ARGS__)
#define LOG_ALERT(s, ...)             zl::base::Logger::log(ZL_LOG_MARK, zl::base::ZL_LOG_PRIO_ALERT,     s, ##__VA_ARGS__)
#define LOG_EMERGENCY(s, ...)         zl::base::Logger::log(ZL_LOG_MARK, zl::base::ZL_LOG_PRIO_EMERGENCY, s, ##__VA_ARGS__)

NAMESPACE_ZL_BASE_END
#endif /* ZL_ZLOG_H */
