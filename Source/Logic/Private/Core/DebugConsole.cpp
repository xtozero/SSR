#include "stdafx.h"
#include "Core/DebugConsole.h"

#include "common.h"
#include "ConsoleMessage/ConsoleMessageExecutor.h"
#include "Util.h"

#include <cstdio>
#include <process.h>

namespace
{
	unsigned int __stdcall asyncInputFunc( void* arg )
	{
		bool& isAlive = *(static_cast<bool*>( arg ));

		while ( isAlive )
		{
			char conMessage[1024] = { 0, };
			gets_s( conMessage, sizeof( conMessage ) );

			if ( strlen( conMessage ) == 0 )
			{
				continue;
			}

			GetConsoleMessageExecutor( ).AppendCommand( conMessage );
		}

		return 0;
	}
}

CDebugConsole::CDebugConsole( ) : m_isAlive( true )
{
	m_thread = (HANDLE)_beginthreadex( nullptr, 0, asyncInputFunc, &m_isAlive, 0, nullptr );

	AllocConsole( );
	freopen_s( &m_pConOut, "CONOUT$", "wt", stdout );
	freopen_s( &m_pConIn, "CONIN$", "r", stdin );
}


CDebugConsole::~CDebugConsole( )
{
	m_isAlive = false;
	WaitForSingleObject( m_thread, INFINITE );
	CloseHandle( m_thread );

	FreeConsole( );
	::fclose( m_pConOut );
	::fclose( m_pConIn );
}