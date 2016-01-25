#include "stdafx.h"
#include "common.h"
#include "../shared/Util.h"
#include "../Engine/ConsoleMessageExecutor.h"
#include <cstdio>
#include "DebugConsole.h"
#include <process.h>

namespace
{
	unsigned int __stdcall asyncInputFunc( void * arg )
	{
		bool* isAlive = static_cast<bool*>( arg );

		char conMessage[1024] = { 0, };

		while ( isAlive )
		{
			gets_s( conMessage, sizeof( conMessage ) );

			TCHAR convertedMessage[1024];
			size_t converted;
			mbstowcs_s( &converted, convertedMessage, conMessage, _countof( convertedMessage ) );

			if ( converted == _countof( convertedMessage ) )
			{
				DebugWarning( _T( "Console Message Buffer Overflowed!!!\n" ) );
				continue;
			}

			GetConsoleMessageExecutor( )->Execute( convertedMessage );
		}

		return 0;
	}
}

CDebugConsole* CDebugConsole::GetInstance( )
{
	static CDebugConsole console;

	return &console;
}

CDebugConsole::CDebugConsole( ) : m_isAlive( true )
{
	HANDLE m_thread = (HANDLE)_beginthreadex( nullptr, 0, asyncInputFunc, &m_isAlive, 0, nullptr );

	AllocConsole( );
	::_tfreopen_s( &m_pConOut, _T( "CONOUT$" ), _T( "wt" ), stdout );
	::_tfreopen_s( &m_pConIn, _T( "CONIN$" ), _T( "r" ), stdin );
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