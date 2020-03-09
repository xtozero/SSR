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

			TCHAR convertedMessage[1024];
			size_t converted;
			mbstowcs_s( &converted, convertedMessage, conMessage, _countof( convertedMessage ) );

			if ( converted == _countof( convertedMessage ) )
			{
				DebugWarning( "Console Message Buffer Overflowed!!!\n" );
				continue;
			}

			GetConsoleMessageExecutor( ).AppendCommand( convertedMessage );
		}

		return 0;
	}
}

CDebugConsole::CDebugConsole( ) : m_isAlive( true )
{
	m_thread = (HANDLE)_beginthreadex( nullptr, 0, asyncInputFunc, &m_isAlive, 0, nullptr );

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