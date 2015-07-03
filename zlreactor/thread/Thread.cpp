#include "thread/Thread.h"
#if defined(OS_WINDOWS)
#include <process.h>
#else
#include <syscall.h>
#endif

NAMESPACE_ZL_THREAD_START

struct ThreadImplDataInfo
{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc            threadFunc_;
    Thread                *thread_;
    native_thread_handle  threadId_;

    ThreadImplDataInfo(const ThreadFunc& func, Thread *thread, native_thread_handle tid)
        : threadFunc_(func), thread_(thread), threadId_(tid)
    {
    }

    void RunThread()
    {
        try
        {
            // Call the actual client thread function
            threadFunc_();
        }
        catch(...)
        {
            // Uncaught exceptions will terminate the application (default behavior according to C++11)
            std::terminate();
        }
    }
};

#if defined(OS_WINDOWS)
unsigned WINAPI StartThread(void *aArg)
#else
void *StartThread(void *aArg)
#endif
{
    ThreadImplDataInfo *data = static_cast<ThreadImplDataInfo *>(aArg);
    data->RunThread();
    delete data;
    return NULL;
}

Thread::Thread(const ThreadFunc& func, const std::string& name/* = unknown*/)
    : threadId_(0)
    , threadFunc_(func)
    , threadName_(name)
    , notAThread_(true)
    , joined_(false)
{

    ThreadImplDataInfo *data = new ThreadImplDataInfo(threadFunc_, this, threadId_);

    // Create the thread
#if defined(OS_WINDOWS)
    threadId_ = (HANDLE) _beginthreadex(0, 0, StartThread, (void *)data, 0, &win32ThreadID_);
#else
    if(pthread_create(&threadId_, NULL, StartThread, (void *)data) != 0)
        threadId_ = 0;
#endif
    if(!threadId_)
    {
        delete data;
        abort();
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
