#include "stdafx.h"
#include "Timer.h"

using namespace D3D11Framework;

bool Timer::Init()
{
    QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
    if (m_frequency == 0)
        return false;

    m_ticksPerMs = (float)(m_frequency / 1000);

    QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

    return true;
}

void Timer::Frame()
{
    INT64 currentTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

    float timeDifference = (float)(currentTime - m_startTime);

    m_frameTime = timeDifference / m_ticksPerMs;
    m_startTime = currentTime;
}

float Timer::GetTime()
{
    return m_frameTime;
}