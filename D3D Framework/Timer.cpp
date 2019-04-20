#include "stdafx.h"
#include "Timer.h"

using namespace D3D11Framework;

Timer::Timer()
{
	m_TimerStart = GetTickCount() / 1000.0f;
	m_TimerGoal = m_TimerStart;
	m_curSet = 0;
}

void Timer::SetTimer(float time)
{
	m_curSet = time;

	m_TimerStart = GetTickCount() / 1000.0f;
	m_TimerGoal = m_TimerStart + m_curSet;
}

void Timer::Restart()
{
	m_TimerStart = GetTickCount() / 1000.0f;
	m_TimerGoal = m_TimerStart + m_curSet;
}

float Timer::GetCurOption()
{
	return m_curSet;
}

bool Timer::Resault()
{
	DWORD curTime = GetTickCount() / 1000.f;
	if (curTime == m_TimerGoal)
	{
		m_curSet = 0;
		return true;
	}
	else
		return false;
}

void Timer::Stop()
{
	m_curSet = 0;
	m_TimerGoal = m_TimerStart;
}