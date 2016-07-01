#include "zlreactor/thread/Thread.h"
#include "zlreactor/base/Exception.h"
#if defined(OS_WINDOWS)
#include <process.h>
#else
#include <syscall.h>
#endif
NAMESPACE_ZL_THREAD_START

//#define DO_NOT_USE_TRY_CATCH

namespace detail
{
    struct ThreadImplDataInfo
    {
        typedef zl::thread::Thread::ThreadFunc ThreadFunc;
        ThreadFunc func_;
        std::string name_;

        ThreadImplDataInfo(const ThreadFunc& func, const std::string& threadName)
            : func_(func)
            , name_(threadName)
        {
            
        }

        void runThread()
        {
        #ifdef DO_NOT_USE_TRY_CATCH
            func_();
        #else
            try
            {
                func_();
            }
            catch (const zl::base::Exception& ex)
            {
                fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
                fprintf(stderr, "reason: %s\n", ex.what());
                fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
                std::abort();
            }
            catch (const std::exception& ex)
            {
                fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
                fprintf(stderr, "reason: %s\n", ex.what());
                std::abort();
            }
            catch (...)
            {
                fprintf(stderr, "uncaught exception caught in Thread %s\n", name_.c_str());
                // Uncaught exceptions will terminate the application (default behavior according to C++11)
                std::terminate();
            }
        #endif
        }
    };

#if defined(OS_WINDOWS)
    unsigned WINAPI startThread(void *arg)
#else
    void* startThread(void *arg)
#endif
    {
        ThreadImplDataInfo *data = static_cast<ThreadImplDataInfo *>(arg);
        data->runThread();
        delete data;
        return 0;
    }
}

Thread::Thread(const ThreadFunc& func, const std::string& name/* = unknown*/)
    : threadId_(0)
    //, threadFunc_(func)
    , threadName_(name)
    , notAThread_(true)
    , joined_(false)
{

    detail::ThreadImplDataInfo* data = new detail::ThreadImplDataInfo(func, name);
    // Create the thread
#if defined(OS_WINDOWS)
    threadId_ = (HANDLE) _beginthreadex(0, 0, detail::startThread, data, 0, &win32ThreadID_);
#else
    if (pthread_create(&threadId_, NULL, detail::startThread, data) != 0)
        threadId_ = 0;
#endif
    if(!threadId_)
    {
        delete data;
        std::abort();
    }
    notAThread_ = false;  // The thread is now alive
}

Thread::~Thread()
{
    if(joinable())
        std::terminate();
}

bool Thread::joinable() const
{
    return !joined_ && !notAThread_;
}

void Thread::join()
{
    if(joinable())
    {
    #if defined(OS_WINDOWS)
        WaitForSingleObject(threadId_, INFINITE);
        CloseHandle(threadId_);
    #else
        pthread_join(threadId_, NULL);
    #endif
        joined_ = true;
    }
}

void Thread::detach()
{
    // It's not use joinable(), so iff you call detach() more than once, then terimnate
    if (!joined_/*joinable()*/)
    {
    #if defined(OS_WINDOWS)
        CloseHandle(threadId_);
    #else
        pthread_detach(threadId_);
    #endif
        notAThread_ = true;
    }
}

Thread::id Thread::get_id() const
{
    if(!joinable())
        return id();
#if defined(OS_WINDOWS)
    return id((unsigned long int) win32ThreadID_);
#else
    return id(threadId_);
#endif
}

/*static*/ unsigned int Thread::hardware_concurrency()
{
#if defined(OS_WINDOWS)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (int) si.dwNumberOfProcessors;
#elif defined(_SC_NPROCESSORS_ONLN)
    return (int) sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_SC_NPROC_ONLN)
    return (int) sysconf(_SC_NPROC_ONLN);
#else
    // The standard requires this function to return zero if the number of
    // hardware cores could not be determined.
    return 0;
#endif
}

//------------------------------------------------------------------------------
// this_thread
//------------------------------------------------------------------------------
namespace this_thread
{
    thread_local int g_currentTid = 0;

    void cacheThreadTid()
    {
    #if defined(OS_WINDOWS)
        g_currentTid = static_cast<int>(::GetCurrentThreadId());
    #else
        g_currentTid = static_cast<int>(::syscall(SYS_gettid));
    #endif
    }
    
    int gettid()
    {
        if (g_currentTid == 0)
        {
            cacheThreadTid();
        }
        return g_currentTid;
    }

    Thread::id get_id()
    {
    #if defined(OS_WINDOWS)
        return Thread::id(static_cast<unsigned long int>(::GetCurrentThreadId()));
    #else
        return Thread::id(pthread_self());
    #endif
    }
}

NAMESPACE_ZL_THREAD_END
