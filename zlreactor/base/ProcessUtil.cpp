#include "ProcessUtil.h"
#include "FileUtil.h"
#include "thread/Thread.h"
#include <dirent.h>
#include <pwd.h>
#include <stdio.h>         // snprintf
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <sys/prctl.h>    // prctl
#include <syscall.h>
NAMESPACE_ZL_START

namespace
{
    Timestamp g_startTime = Timestamp::now();
    int g_clockTicks = static_cast<int>(::sysconf(_SC_CLK_TCK));
    int g_pageSize = static_cast<int>(::sysconf(_SC_PAGE_SIZE));

    __thread int g_numOpenedFiles = 0;
    __thread std::vector<pid_t>* g_pids = NULL;

    int fdDirFilter(const struct dirent* d)
    {
        if (::isdigit(d->d_name[0]))
        {
            ++g_numOpenedFiles;
        }
        return 0;
    }

    int taskDirFilter(const struct dirent* d)
    {
        if (::isdigit(d->d_name[0]))
        {
            g_pids->push_back(atoi(d->d_name));
        }
        return 0;
    }

    int scanDir(const char *dirpath, int (*filter)(const struct dirent *))
    {
        struct dirent** namelist = NULL;
        int result = ::scandir(dirpath, &namelist, filter, alphasort);
        assert(namelist == NULL);
        return result;
    }
}

namespace ProcessUtil
{
    pid_t pid()
    {
	#ifdef OS_WINDOWS
		return ::GetCurrentProcessId();
	#else
        return ::getpid();
	#endif
    }

    string pidString()
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d", pid());
        return buf;
    }

    string procname()
    {
        //return procname(procStat());
        string name;
        char buf[512] = {0};
    #ifdef OS_WINDOWS
        if (::GetModuleFileNameA(NULL, buf, 512) > 0)
            name = buf;
        std::string::size_type pos = name.rfind("\\");
        if (pos != std::string::npos)
            name = name.substr(pos+1, std::string::npos);
        pos = name.rfind(".");
        if (pos != std::string::npos)
            name = name.substr(0, pos-0);
    #else
        sprintf(buf, "/proc/%d/cmdline", (int)getpid());
        readFile(buf, name);
        std::string::size_type pos = name.rfind("/");
        if (pos != std::string::npos)
        {
            name = name.substr(pos+1, std::string::npos);
        }
    #endif
        return name;
    }

    string procname(const string& stat)
    {
        string name;
        size_t lp = stat.find('(');
        size_t rp = stat.rfind(')');
        if (lp != string::npos && rp != string::npos && lp < rp)
        {
            name = stat.substr(lp+1, (rp-lp-1));
        }
        return name;
    }

    uid_t uid()
    {
        return ::getuid();
    }

    uid_t euid()
    {
        return ::geteuid();
    }

    string username()
    {
        struct passwd pwd;
        struct passwd* result = NULL;
        char buf[8192];
        const char* name = "unknownuser";

        getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
        if (result)
        {
            name = pwd.pw_name;
        }
        return name;
    }

    Timestamp startTime()
    {
        return g_startTime;
    }

    int64_t elapsedTime()
    {
        return Timestamp::timeDiffMs(Timestamp::now(), g_startTime);
    }

    int clockTicksPerSecond()
    {
        return g_clockTicks;
    }

    int pageSize()
    {
        return g_pageSize;
    }

    string hostname()
    {
        char buf[256];
        if (::gethostname(buf, sizeof(buf)) == 0)
        {
            buf[sizeof(buf)-1] = '\0';
            return buf;
        }
        else
        {
            return "unknownhost";
        }
    }

    string procStatus()
    {
        string result;
        readFile("/proc/self/status", result);
        return result;
    }

    string procStat()
    {
        string result;
        readFile("/proc/self/stat", result);
        return result;
    }

    string threadStat()
    {
        char buf[64];
        snprintf(buf, sizeof buf, "/proc/self/task/%d/stat", thread::this_thread::tid());
        string result;
        readFile(buf, result);
        return result;
    }

    string exePath()
    {
        return FileUtil::getBinaryPath();
    }

    int openedFiles()
    {
        g_numOpenedFiles = 0;
        scanDir("/proc/self/fd", fdDirFilter);
        return g_numOpenedFiles;
    }

    int maxOpenFiles()
    {
        // return getdtablesize();
        struct rlimit rl;
        if (::getrlimit(RLIMIT_NOFILE, &rl))
        {
            return openedFiles();
        }
        else
        {
            return static_cast<int>(rl.rlim_cur);
        }
    }

    CpuTime cpuTime()
    {
        CpuTime t;
        struct tms tms;
        if (::times(&tms) >= 0)
        {
            const double hz = static_cast<double>(clockTicksPerSecond());
            t.userSeconds = static_cast<double>(tms.tms_utime) / hz;
            t.systemSeconds = static_cast<double>(tms.tms_stime) / hz;
        }
        return t;
    }

    int numThreads()
    {
        int result = 0;
        string status = procStatus();
        size_t pos = status.find("Threads:");
        if (pos != string::npos)
        {
            result = ::atoi(status.c_str() + pos + 8);
        }
        return result;
    }

    std::vector<pid_t> threads()
    {
        std::vector<pid_t> result;
        g_pids = &result;
        scanDir("/proc/self/task", taskDirFilter);
        g_pids = NULL;
        std::sort(result.begin(), result.end());
        return result;
    }

    bool enableCoreDump(bool enabled/* = true*/, int core_file_size/* = -1*/)
    {
        if (enabled)
        {
            struct rlimit rlim;
            rlim.rlim_cur = (core_file_size < 0)? RLIM_INFINITY: core_file_size;
            rlim.rlim_max = rlim.rlim_cur;

            if (-1 == ::setrlimit(RLIMIT_CORE, &rlim))
                return false;
        }       
        
        if (-1 == ::prctl(PR_SET_DUMPABLE, enabled ? 1 : 0))
            return false;

        return true;
    }
}
NAMESPACE_ZL_END
