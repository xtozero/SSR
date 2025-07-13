#pragma once

#include <cstdio>

namespace logic
{
	class DebugConsole
	{
	public:
		DebugConsole();
		~DebugConsole();

	private:
		HANDLE m_thread;
		FILE* m_pConOut;
		FILE* m_pConIn;
	};
}
