#ifndef HIGHPRECISIONTIME_FILE_H
#define HIGHPRECISIONTIME_FILE_H
#include "zlreactor/Define.h"
#ifdef OS_WINDOWS
#include <Windows.h>
#endif
NAMESPACE_ZL_BASE_START

class HighPrecisionTime
{
public:
    HighPrecisionTime()
    {
        ::QueryPerformanceFrequency(&m_liPerfFreq);
        start();
    }

    void reset()
    {
        LARGE_INTEGER liPerfNow;
        ::QueryPerformanceCounter(&liPerfNow);
        m_liPerfStart = liPerfNow;
    }

    long long now()
    {
        LARGE_INTEGER liPerfNow;
        ::QueryPerformanceCounter(&liPerfNow);
        return liPerfNow.QuadPart;
    }

    long long getFreq()
    {
        return m_liPerfFreq.QuadPart;
    }

    double elapsedTime()
    {
        LARGE_INTEGER liPerfNow;
        ::QueryPerformanceCounter(&liPerfNow);

        return ((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1.0) / m_liPerfFreq.QuadPart ;
    }

    double elapsedTimeInMill()
    {
        LARGE_INTEGER liPerfNow;
        ::QueryPerformanceCounter(&liPerfNow);

        return ((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000.0) / m_liPerfFreq.QuadPart ;
    }
    double elapsedTimeInMicro()
    {
        LARGE_INTEGER liPerfNow;
        ::QueryPerformanceCounter(&liPerfNow);

        return ((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000000.0) / m_liPerfFreq.QuadPart ;
    }
private:
    void start()
    {
        ::QueryPerformanceCounter(&m_liPerfStart);
    }

private:
    LARGE_INTEGER	m_liPerfFreq;	//Conuts per second
    LARGE_INTEGER	m_liPerfStart;	//Starting count
};

NAMESPACE_ZL_BASE_END
#endif /* HIGHPRECISIONTIME_FILE_H */