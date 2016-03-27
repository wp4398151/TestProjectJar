#include "TimerClass.h"

TimerClass::TimerClass()
{

}

TimerClass::TimerClass(const TimerClass& rTimeClass)
{

}

TimerClass::~TimerClass()
{

}

bool TimerClass::Initialize()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0)
	{
		return false;
	}
	m_ticksPerMs = (float)(m_frequency / 1000);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);
	return true;
}

void TimerClass::Frame()
{
	INT64 currentTime = 0;
	float timeDifference = 0.0f;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	timeDifference = (float)(currentTime - m_startTime);
	m_frameTime = timeDifference / m_ticksPerMs;
	m_startTime = currentTime;
}

float TimerClass::GetTime()
{
	return m_frameTime;
}