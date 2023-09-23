#include "stdafx.h"
#include "Core/Timer.h"

#include "common.h"
#include "ConsoleMessage/ConVar.h"

#include <windows.h>

using ::std::chrono::duration;
using ::std::chrono::nanoseconds;
using ::std::chrono::steady_clock;
using ::std::chrono::time_point;

namespace logic
{
	void Timer::Tick()
	{
		++m_frame;
		++m_totalFrameCount;

		time_point<steady_clock> curTime = steady_clock::now();

		m_elapsedTime = curTime - m_lastTime;
		m_lastTime = curTime;

		m_frameCheckInterval += duration_cast<duration<float>>( m_elapsedTime ).count();

		if ( m_frameCheckInterval > 1.0f )
		{
			m_frameCheckInterval = 0.f;
			m_fps = static_cast<float>( m_frame );
			m_frame = 0;
		}

		auto scaledTime = duration_cast<duration<float>>( m_elapsedTime ) * m_timeScale;
		m_elapsedTime = round<nanoseconds>( scaledTime );

		if ( m_isPaused )
		{
			m_elapsedTime = nanoseconds::zero();
		}
		else
		{
			m_totalTime += m_elapsedTime;
		}
	}

	void Timer::Pause()
	{
		m_isPaused = true;
	}

	void Timer::Resume()
	{
		m_isPaused = false;
	}

	float Timer::GetElapsedTime() const
	{
		return duration_cast<duration<float>>( m_elapsedTime ).count();
	}

	float Timer::GetTotalTime() const
	{
		return duration_cast<duration<float>>( m_totalTime ).count();
	}

	uint32 Timer::GetTotalFrameCount() const
	{
		return m_totalFrameCount;
	}

	Timer::Timer()
	{
		m_lastTime = steady_clock::now();
	}
}
