#include "stdafx.h"
#include "UtilLibrary.h"

#pragma warning(disable : 4996)

static LARGE_INTEGER cpuFreq;
static LARGE_INTEGER startTime;
static LARGE_INTEGER endTime;

UTILLIBRARY_API uint32_t TickStart()
{
    QueryPerformanceFrequency(&cpuFreq);
    QueryPerformanceCounter(&startTime);

    return 0;
}

UTILLIBRARY_API uint32_t TickEnd(uint32_t &tickTime)
{
    QueryPerformanceCounter(&endTime);
    tickTime = (uint32_t)(((endTime.QuadPart - startTime.QuadPart) * 1000000.0) / cpuFreq.QuadPart);

    return 0;
}

UTILLIBRARY_API uint32_t DelayUs(double dbTimeUs)
{
    LARGE_INTEGER CurrTicks, TicksCount;

    QueryPerformanceFrequency(&TicksCount);
    QueryPerformanceCounter(&CurrTicks);

    TicksCount.QuadPart = ((LONGLONG)(TicksCount.QuadPart * dbTimeUs)) / 1000000i64;
    TicksCount.QuadPart += CurrTicks.QuadPart;

    while (CurrTicks.QuadPart < TicksCount.QuadPart) {
        QueryPerformanceCounter(&CurrTicks);
    }

    return 0;
}

UTILLIBRARY_API uint32_t DelayMs(double dbTimeMs)
{
    LARGE_INTEGER CurrTicks, TicksCount;

    QueryPerformanceFrequency(&TicksCount);
    QueryPerformanceCounter(&CurrTicks);

    TicksCount.QuadPart = ((LONGLONG)(TicksCount.QuadPart * dbTimeMs)) / 1000;
    TicksCount.QuadPart += CurrTicks.QuadPart;

    while (CurrTicks.QuadPart < TicksCount.QuadPart) {
        QueryPerformanceCounter(&CurrTicks);
    }

    return 0;
}
