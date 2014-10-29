#include "thread/Thread.h"
#if defined(OS_WINDOWS)
#include <process.h>
#else
#include <unistd.h>
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
        // The thread is no longer executing
        MutexLocker guard(thread_->threadMutex_);
        thread_->notAThread = true;
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
    : threadId_(0), threadFunc_(func), threadName_(name), notAThread(true)
{
    ThreadImplDataInfo *data = new ThreadImplDataInfo(threadFunc_, this, threadId_);
    // Create the thread
#if defined(OS_WINDOWS)
    threadId_ = (HANDLE) _beginthreadex(0, 0, StartThread, (void *)data, 0, &win32ThreadID_);
#elif defined(_TTHREAD_POSIX_)
    if(pthread_create(&threadId_, NULL, StartThread, (void *)data) != 0)
        threadId_ = 0;
#endif
    if(!threadId_)
    {
        delete data;
        abort();
    }
    // The thread is now alive
    notAThread = false;
}

Thread::~Thread()
{
    if(joinable())
        std::terminate();
}

void Thread::join()
{
    if(joinable())
    {
#if defined(OS_WINDOWS)
        WaitForSingleObject(threadId_, INFINITE);
        CloseHandle(threadId_);
#elif defined(_TTHREAD_POSIX_)
        pthread_join(threadId_, NULL);
#endif
    }
}

bool Thread::joinable() const
{
    MutexLocker lock(threadMutex_);
    return !notAThread;
}

void Thread::detach()
{
    MutexLocker lock(threadMutex_);
    if(!notAThread)
    {
#if defined(OS_WINDOWS)
        CloseHandle(threadId_);
#elif defined(_TTHREAD_POSIX_)
        pthread_detach(threadId_);
#endif
        notAThread = true;
    }
}

Thread::id Thread::get_id() const
{
    if(!joinable())
        return id();
#if defined(OS_WINDOWS)
    return id((unsigned long int) win32ThreadID_);
#elif defined(_TTHREAD_POSIX_)
    return _pthread_t_to_ID(mHandle);
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
Thread::id this_thread::get_id()
{
#if defined(OS_WINDOWS)
    return Thread::id((unsigned long int) GetCurrentThreadId());
#elif defined(_TTHREAD_POSIX_)
    return _pthread_t_to_ID(pthread_self());
#endif
}

NAMESPACE_ZL_THREAD_END
