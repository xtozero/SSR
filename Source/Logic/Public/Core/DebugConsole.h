#pragma once

#include <cstdio>

namespace logic
{
	class CDebugConsole
	{
	private:
		HANDLE	m_thread;
		bool	m_isAlive;
		FILE* m_pConOut;
		FILE* m_pConIn;
	public:

		CDebugConsole();
		~CDebugConsole();
	};
}
