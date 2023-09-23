#pragma once

#include "SizedTypes.h"

#include <chrono>

namespace logic
{
	class Timer
	{
	public:
		void Tick();
		void Pause();
		void Resume();

		float GetElapsedTime() const;
		float GetTotalTime() const;
		uint32 GetTotalFrameCount() const;
		float GetFps() const { return m_fps; }

		bool IsPaused() const { return m_isPaused; }

		Timer();

	private:
		std::chrono::nanoseconds m_totalTime = std::chrono::nanoseconds::zero();
		std::chrono::nanoseconds m_elapsedTime = std::chrono::nanoseconds::zero();
		float m_timeScale = 1.f;

		std::chrono::time_point<std::chrono::steady_clock> m_lastTime;

		uint32 m_frame = 0;
		uint32 m_totalFrameCount = 0;
		float m_fps = 0.f;
		float m_frameCheckInterval = 0.f;
		bool m_isPaused = false;
	};
}
