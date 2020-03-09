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

	CDebugConsole ();
	~CDebugConsole ();
};
