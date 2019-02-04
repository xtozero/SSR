#pragma once

#include <chrono>

class CTimer
{
public:
	static CTimer& GetInstance( );

	void Tick( );
	void Pause( );
	void Resume( );

	float GetElapsedTime( ) const { return m_elapsedTime; }
	float GetTotalTime( ) const { return m_totalTime; }
	float GetFps( ) const { return m_fps; }

	bool IsPaused( ) const { return m_isPaused; }

	CTimer( );

private:
	float m_totalTime = 0.f;
	float m_elapsedTime = 0.f;

	std::chrono::time_point<std::chrono::steady_clock> m_lastTime;

	int m_frame = 0;
	float m_fps = 0.f;
	float m_frameCheckInterval = 0.f;
	bool m_isPaused = false;
};

