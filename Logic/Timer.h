#pragma once
class CTimer
{
private:
	long long m_currentTime;
	long long m_lastTime;

	float m_elapsedTime;

	LARGE_INTEGER m_timeFrequency;
	LARGE_INTEGER m_timeCouter;

	bool m_isPerformanceCounter;

	int m_frame;
	float m_fps;
	float m_frameCheckInterval;
	float m_timeScale;
public:
	static CTimer& GetInstance( );

	void Tick ();
	inline float GetElapsedTIme( ) const { return m_elapsedTime; }
	inline float GetFps( ) const { return m_fps; }

	CTimer( );
	~CTimer( );
};

