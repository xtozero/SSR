#pragma once

#include <chrono>

class CTimer
{
public:
	static CTimer& GetInstance( );

	void Tick( );
	void Pause( );
	void Resume( );

	inline float GetElapsedTIme( ) const { return m_elapsedTime; }
	inline float GetTotalTime( ) const { return m_totalTime; }
	inline float GetFps( ) const { return m_fps; }

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

