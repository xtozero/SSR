#pragma once
class CTimer
{
private:
	float m_currentTime;
	float m_lastTime;

	float m_elapsedTime;

	LARGE_INTEGER m_timeFrequency;
	LARGE_INTEGER m_timeCouter;

	bool m_isPerformanceCounter;

	int m_frame;
public:
	static CTimer& GetInstance ();

	void Tick ();
	inline float GetElapsedTIme () { return m_elapsedTime; }

	CTimer ( );
	~CTimer ( );
};

