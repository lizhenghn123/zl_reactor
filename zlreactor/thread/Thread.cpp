#include "thread/Thread.h"
#include "base/Exception.h"
#if defined(OS_WINDOWS)
#include <process.h>
#else
#include <syscall.h>
#endif
NAMESPACE_ZL_THREAD_START

Thread::Thread(const ThreadFunc& func, const std::string& name/* = unknown*/)
    : threadId_(0)
    , threadFunc_(func)
    , threadName_(name)
    , notAThread_(true)
    , joined_(false)
{

    // Create the thread
#if defined(OS_WINDOWS)
    threadId_ = (HANDLE) _beginthreadex(0, 0, startThread, this, 0, &win32ThreadID_);
#else
    if (pthread_create(&threadId_, NULL, startThread, this) != 0)
        threadId_ = 0;
#endif
    if(!threadId_)
    {
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

#if defined(OS_WINDOWS)
unsigned WINAPI Thread::startThread(void *aArg)
#else
void* Thread::startThread(void *aArg)
#endif
{
    Thread *trd = static_cast<Thread *>(aArg);
    trd->runThreadFunc();
    return 0;
}

//#define DO_NOT_USE_EXCEPTION

void Thread::runThreadFunc()
{
#ifdef DO_NOT_USE_EXCEPTION
    threadFunc_();
#else
    try
    {
        threadFunc_();
    }
    catch (const zl::base::Exception& ex)
    {
        fprintf(stderr, "exception caught in Thread %s\n", threadName_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
        std::abort();
    }
    catch (const std::exception& ex)
    {
        fprintf(stderr, "exception caught in Thread %s\n", threadName_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        std::abort();
    }
    catch (...)
    {
        fprintf(stderr, "uncaught exception caught in Thread %s\n", threadName_.c_str());
        // Uncaught exceptions will terminate the application (default behavior according to C++11)
        std::terminate();
    }
#endif
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

    int gettid()
    {
    #if defined(OS_WINDOWS)
        return static_cast<int>(::GetCurrentThreadId());
    #else
        return static_cast<int>(::syscall(SYS_gettid));
    #endif
    }

    void cacheThreadTid()
    {
        if (g_currentTid == 0)
        {
            g_currentTid = gettid();
        }
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
