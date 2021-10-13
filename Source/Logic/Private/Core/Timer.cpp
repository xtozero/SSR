#include "stdafx.h"
#include "Core/Timer.h"

#include "common.h"
#include "ConsoleMessage/ConVar.h"

#include <windows.h>

using namespace std::chrono;

void CTimer::Tick ( )
{
	++m_frame;

	time_point<steady_clock> curTime = steady_clock::now( );

	m_elapsedTime = duration_cast<duration<float>>( curTime - m_lastTime ).count();
	m_lastTime = curTime;

	m_frameCheckInterval += m_elapsedTime;
	
	if ( m_frameCheckInterval > 1.0f )
	{
		m_frameCheckInterval = 0.f;
		m_fps = static_cast<float>( m_frame );
		m_frame = 0;
	}

	m_elapsedTime *= m_timeScale;

	if ( m_isPaused )
	{
		m_elapsedTime = 0;
	}
	else
	{
		m_totalTime += m_elapsedTime;
	}
}

void CTimer::Pause( )
{
	m_isPaused = true;
}

void CTimer::Resume( )
{
	m_isPaused = false;
}

CTimer::CTimer ( )
{
	m_lastTime = steady_clock::now( );
}