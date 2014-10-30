#include "base/Timestamp.h"
#include <stdio.h>
#include "Define.h"
#include <time.h>
#ifdef OS_WINDOWS
#include <Windows.h>
#else
#include <sys/time.h>
#endif

namespace zl{
namespace base {

Timestamp::Timestamp() : microSeconds_(0)
{

}

Timestamp::Timestamp(int64_t ms) : microSeconds_(ms)
{

}

/*static*/ Timestamp Timestamp::now()
{
#ifdef OS_WINDOWS
    //Number of micro-seconds between the beginning of the Windows epoch (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970)
    const long long win_epoch_filetime = 116444736000000000ULL;

    FILETIME ft;
    LARGE_INTEGER li;
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;

    int64_t tt = (li.QuadPart - win_epoch_filetime) / 10;
    return Timestamp(tt);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return Timestamp(tv.tv_sec * ZL_USEC_PER_SEC + tv.tv_usec);
#endif
}

struct tm  *Timestamp::getTm(bool showlocaltime/* = true*/)
{
    time_t seconds = static_cast<time_t>(microSeconds_ / ZL_USEC_PER_SEC);
    if(showlocaltime)
        return localtime(&seconds);
    else
        return gmtime(&seconds);
}

std::string Timestamp::toString(bool showlocaltime/* = true*/)
{
    struct tm *tm_time = NULL;
    time_t seconds = static_cast<time_t>(microSeconds_ / ZL_USEC_PER_SEC);
    int microseconds = microSeconds_ % (ZL_USEC_PER_SEC);

    if(showlocaltime)
        tm_time = localtime(&seconds);
    else
        tm_time = gmtime(&seconds);

    char buf[32] = { 0 };
	ZL_SNPRINTF(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d:%06d", tm_time->tm_year + 1900,
              tm_time->tm_mon + 1, tm_time->tm_mday, tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec, microseconds);

    return buf;
}

} // namespace base
} // namespace zl