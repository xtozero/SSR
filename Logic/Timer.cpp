#include "stdafx.h"
#include "common.h"
#include "Timer.h"
#include <windows.h>

#pragma comment( lib, "winmm.lib" )

CTimer& CTimer::GetInstance ( )
{
	static CTimer timer;

	return timer;
}

void CTimer::Tick ( )
{
	++m_frame;
	m_lastTime = m_currentTime;

	if ( m_isPerformanceCounter )
	{
		::QueryPerformanceCounter ( &m_timeCouter );
		m_currentTime = static_cast<float>( m_timeCouter.QuadPart ) / m_timeFrequency.QuadPart;
	}
	else
	{
		m_currentTime = static_cast<float>( GetTickCount ( ) ) / m_timeFrequency.QuadPart;
	}

	m_elapsedTime = m_currentTime - m_lastTime;

//	DebugMsg ( "Elapsted Time - %f\n", m_elapsedTime );
}

CTimer::CTimer ( ) : m_currentTime ( 0.0f ), 
m_lastTime ( 0.0f ), 
m_elapsedTime ( 0.0f ),
m_frame( 0 )
{
	::QueryPerformanceCounter ( &m_timeCouter );

	m_isPerformanceCounter = m_timeCouter.QuadPart == 0 ? false : true;

	if ( m_isPerformanceCounter )
	{
		::QueryPerformanceFrequency ( &m_timeFrequency );
	}
	else
	{
		m_timeFrequency.QuadPart = 1000;
	}
}

CTimer::~CTimer ( )
{
}