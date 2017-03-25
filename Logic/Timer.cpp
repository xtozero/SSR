#include "stdafx.h"
#include "common.h"
#include "Timer.h"
#include "../shared/Util.h"
#include "../Engine/ConVar.h"
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
		m_currentTime = m_timeCouter.QuadPart;
	}
	else
	{
		m_currentTime = GetTickCount64( );
	}

	m_elapsedTime = static_cast<float>( m_currentTime - m_lastTime ) * m_timeScale;

	m_frameCheckInterval += m_elapsedTime;
	
	if ( m_frameCheckInterval > 1.0f )
	{
		m_frameCheckInterval = 0.f;
		m_fps = static_cast<float>( m_frame );
		m_frame = 0;
	}
}

CTimer::CTimer ( ) : m_currentTime ( 0 ), 
m_lastTime ( 0 ), 
m_elapsedTime ( 0.f ),
m_frame( 0 ),
m_fps( 0.f ),
m_frameCheckInterval( 0.f )
{
	::QueryPerformanceCounter( &m_timeCouter );

	m_isPerformanceCounter = m_timeCouter.QuadPart == 0 ? false : true;

	if ( m_isPerformanceCounter )
	{
		m_currentTime = m_timeCouter.QuadPart;
		::QueryPerformanceFrequency ( &m_timeFrequency );
		m_timeScale = 1.f / m_timeFrequency.QuadPart;
	}
	else
	{
		m_timeScale = 1.f / 1000.f;
	}
}

CTimer::~CTimer ( )
{
}