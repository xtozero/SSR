#include "stdafx.h"
#include "common.h"
#include <cstdio>
#include "DebugConsole.h"
#include <process.h>

namespace
{
	unsigned int __stdcall asyncInputFunc ( void * arg )
	{
		bool* isAlive = static_cast<bool*>( arg );

		char buff[1024] = {0,};

		while ( isAlive )
		{
			gets_s ( buff, sizeof( buff ) );
			DebugMsg ( "%s\n", buff );
		}

		return 0;
	}
}

CDebugConsole* CDebugConsole::GetInstance ()
{
	static CDebugConsole console;

	return &console;
}

CDebugConsole::CDebugConsole ( ) : m_isAlive ( true )
{
	HANDLE m_thread = (HANDLE)_beginthreadex( nullptr, 0, asyncInputFunc, &m_isAlive, 0, nullptr );

	AllocConsole ();
	::freopen_s ( &m_pConOut, "CONOUT$", "wt", stdout );
	::freopen_s ( &m_pConIn, "CONIN$", "r", stdin );
}


CDebugConsole::~CDebugConsole ()
{
	m_isAlive = false;
	WaitForSingleObject ( m_thread, INFINITE );
	CloseHandle ( m_thread );

	FreeConsole ();
	::fclose ( m_pConOut );
	::fclose ( m_pConIn );
}