// ***********************************************************************
// Filename         : ProcessTimeCounter.h
// Author           : LIZHENG
// Created          : 2014-09-24
// Description      : 与进程相关的性能统计工具
//                  
// Last Modified By : LIZHENG
// Last Modified On : 2014-09-24
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_PROCESSTIMECOUNTER_H
#define ZL_PROCESSTIMECOUNTER_H
#include "Define.h"
#ifdef OS_LINUX
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#else
#include <Windows.h>
#endif
NAMESPACE_ZL_BASE_START

class ProcessTimeCounter
{
public:
#ifdef OS_LINUX
	typedef int64_t             interval_type;
#else
	typedef __int64             interval_type;
#endif

public:
	ProcessTimeCounter();

public:
	void        start();
	void        stop();

public:
	// kernel
	/// \brief The elapsed count in the measurement period for kernel mode activity
	///
	/// This represents the extent, in machine-specific increments, of the measurement period for kernel mode activity
	interval_type   kernelPeriodCount() const;
	/// \brief The number of whole seconds in the measurement period for kernel mode activity
	///
	/// This represents the extent, in whole seconds, of the measurement period for kernel mode activity
	interval_type   kernelSeconds() const;
	/// \brief The number of whole milliseconds in the measurement period for kernel mode activity
	///
	/// This represents the extent, in whole milliseconds, of the measurement period for kernel mode activity
	interval_type   kernelMillSeconds() const;
	/// \brief The number of whole microseconds in the measurement period for kernel mode activity
	///
	/// This represents the extent, in whole microseconds, of the measurement period for kernel mode activity
	interval_type   kernelMicroseconds() const;

	// user
	/// \brief The elapsed count in the measurement period for user mode activity
	///
	/// This represents the extent, in machine-specific increments, of the measurement period for user mode activity
	interval_type   userPeriodCount() const;
	/// \brief The number of whole seconds in the measurement period for user mode activity
	///
	/// This represents the extent, in whole seconds, of the measurement period for user mode activity
	interval_type   userSeconds() const;
	/// \brief The number of whole milliseconds in the measurement period for user mode activity
	///
	/// This represents the extent, in whole milliseconds, of the measurement period for user mode activity
	interval_type   userMillSeconds() const;
	/// \brief The number of whole microseconds in the measurement period for user mode activity
	///
	/// This represents the extent, in whole microseconds, of the measurement period for user mode activity
	interval_type   userMicroSeconds() const;

	// Total
	/// \brief The elapsed count in the measurement period
	///
	/// This represents the extent, in machine-specific increments, of the measurement period
	interval_type   periodCount() const;
	/// \brief The number of whole seconds in the measurement period
	///
	/// This represents the extent, in whole seconds, of the measurement period
	interval_type   seconds() const;
	/// \brief The number of whole milliseconds in the measurement period
	///
	/// This represents the extent, in whole milliseconds, of the measurement period
	interval_type   millSeconds() const;
	/// \brief The number of whole microseconds in the measurement period
	///
	/// This represents the extent, in whole microseconds, of the measurement period
	interval_type   microSeconds() const;

private:
#ifdef OS_LINUX
	typedef struct timeval timeval_t;
#else
	typedef __int64        timeval_t;
	static HANDLE   processHandle();
#endif
	timeval_t   m_kernelStart;
	timeval_t   m_kernelEnd;
	timeval_t   m_userStart;
	timeval_t   m_userEnd;
};

inline ProcessTimeCounter::ProcessTimeCounter()
{
	// Note that the constructor does nothing, for performance reasons. Calling
	// any of the Attribute methods before having gone through a start()-stop()
	// cycle will yield undefined results.
}

#if defined(OS_LINUX)
inline void ProcessTimeCounter::start()
{
	struct rusage   r_usage;

	::getrusage(RUSAGE_SELF, &r_usage);

	m_kernelStart   =   r_usage.ru_stime;
	m_userStart     =   r_usage.ru_utime;
}

inline void ProcessTimeCounter::stop()
{
	struct rusage   r_usage;

	::getrusage(RUSAGE_SELF, &r_usage);

	m_kernelEnd     =   r_usage.ru_stime;
	m_userEnd       =   r_usage.ru_utime;
}

// kernel
inline ProcessTimeCounter::interval_type ProcessTimeCounter::kernelPeriodCount() const
{
	return kernelmicroseconds();
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::kernelSeconds() const
{
	assert("end before start: stop() must be called after start()" && m_kernelStart.tv_sec <= m_kernelEnd.tv_sec);

	long    secs    =   m_kernelEnd.tv_sec - m_kernelStart.tv_sec;
	long    usecs   =   m_kernelEnd.tv_usec - m_kernelStart.tv_usec;

	assert(usecs >= 0 || secs > 0);

	return secs + usecs / (1000 * 1000);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::kernelMillSeconds() const
{
	assert("end before start: stop() must be called after start()" && m_kernelStart.tv_sec <= m_kernelEnd.tv_sec);

	long    secs    =   m_kernelEnd.tv_sec - m_kernelStart.tv_sec;
	long    usecs   =   m_kernelEnd.tv_usec - m_kernelStart.tv_usec;

	assert(usecs >= 0 || secs > 0);

	return secs * 1000 + usecs / 1000;
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::kernelMicroseconds() const
{
	assert("end before start: stop() must be called after start()" && m_kernelStart.tv_sec <= m_kernelEnd.tv_sec);

	long    secs    =   m_kernelEnd.tv_sec - m_kernelStart.tv_sec;
	long    usecs   =   m_kernelEnd.tv_usec - m_kernelStart.tv_usec;

	assert(usecs >= 0 || secs > 0);

	return secs * (1000 * 1000) + usecs;
}

// user
inline ProcessTimeCounter::interval_type ProcessTimeCounter::userPeriodCount() const
{
	return usermicroSeconds();
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::userSeconds() const
{
	assert("end before start: stop() must be called after start()" && m_userStart.tv_sec <= m_userEnd.tv_sec);

	long    secs    =   m_userEnd.tv_sec - m_userStart.tv_sec;
	long    usecs   =   m_userEnd.tv_usec - m_userStart.tv_usec;

	assert(usecs >= 0 || secs > 0);

	return secs + usecs / (1000 * 1000);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::userMillSeconds() const
{
	assert("end before start: stop() must be called after start()" &&  m_userStart.tv_sec <= m_userEnd.tv_sec);

	long    secs    =   m_userEnd.tv_sec - m_userStart.tv_sec;
	long    usecs   =   m_userEnd.tv_usec - m_userStart.tv_usec;

	assert(usecs >= 0 || secs > 0);

	return secs * 1000 + usecs / 1000;
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::userMicroSeconds() const
{
	assert("end before start: stop() must be called after start()" &&  m_userStart.tv_sec <= m_userEnd.tv_sec);

	long    secs    =   m_userEnd.tv_sec - m_userStart.tv_sec;
	long    usecs   =   m_userEnd.tv_usec - m_userStart.tv_usec;

	assert(usecs >= 0 || secs > 0);

	return secs * (1000 * 1000) + usecs;
}

// Total
inline ProcessTimeCounter::interval_type ProcessTimeCounter::periodCount() const
{
	return kernelPeriodCount() + userPeriodCount();
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::seconds() const
{
	return periodCount() / interval_type(10000000);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::millSeconds() const
{
	return periodCount() / interval_type(10000);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::microSeconds() const
{
	return PeriodCount() / interval_type(10);
}

#elif defined(OS_WINDOWS)
inline /* static */ HANDLE ProcessTimeCounter::processHandle()
{
	static HANDLE   s_hProcess = ::GetCurrentProcess();
	return s_hProcess;
}

inline void ProcessTimeCounter::start()
{
	FILETIME    creationTime;
	FILETIME    exitTime;

	::GetProcessTimes(processHandle(), &creationTime, &exitTime, reinterpret_cast<LPFILETIME>(&m_kernelStart), reinterpret_cast<LPFILETIME>(&m_userStart));
}

inline void ProcessTimeCounter::stop()
{
	FILETIME    creationTime;
	FILETIME    exitTime;

	::GetProcessTimes(processHandle(), &creationTime, &exitTime, reinterpret_cast<LPFILETIME>(&m_kernelEnd), reinterpret_cast<LPFILETIME>(&m_userEnd));
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::kernelPeriodCount() const
{
	return static_cast<interval_type>(m_kernelEnd - m_kernelStart);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::kernelSeconds() const
{
	return kernelPeriodCount() / interval_type(10000000);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::kernelMillSeconds() const
{
	return kernelPeriodCount() / interval_type(10000);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::kernelMicroseconds() const
{
	return kernelPeriodCount() / interval_type(10);
}

// user
inline ProcessTimeCounter::interval_type ProcessTimeCounter::userPeriodCount() const
{
	return static_cast<interval_type>(m_userEnd - m_userStart);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::userSeconds() const
{
	return userPeriodCount() / interval_type(10000000);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::userMillSeconds() const
{
	return userPeriodCount() / interval_type(10000);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::userMicroSeconds() const
{
	return userPeriodCount() / interval_type(10);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::periodCount() const
{
	return kernelPeriodCount() + userPeriodCount();
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::seconds() const
{
	return periodCount() / interval_type(10000000);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::millSeconds() const
{
	return periodCount() / interval_type(10000);
}

inline ProcessTimeCounter::interval_type ProcessTimeCounter::microSeconds() const
{
	return periodCount() / interval_type(10);
}

#endif 

NAMESPACE_ZL_BASE_END
#endif  /* ZL_PROCESSTIMECOUNTER_H */