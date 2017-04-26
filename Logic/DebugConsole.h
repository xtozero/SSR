#pragma once

#include <cstdio>

class CDebugConsole
{
private:
	HANDLE	m_thread;
	bool	m_isAlive;
	FILE*	m_pConOut;
	FILE*	m_pConIn;
public:
	static CDebugConsole* GetInstance ();

	CDebugConsole ();
	~CDebugConsole ();
};

#ifdef DEBUGGING_BY_CONSOLE
#define ShowDebugConsole CDebugConsole::GetInstance
#else
#define ShowDebugConsole __noop
#endif