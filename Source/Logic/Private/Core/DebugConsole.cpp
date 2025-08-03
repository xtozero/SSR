#include "Core/DebugConsole.h"

#include "ConsoleMessage/ConsoleMessageExecutor.h"
#include "DebugUtil.h"

#include <cstdio>
#include <process.h>

namespace
{
	uint32 __stdcall asyncInputFunc( [[maybe_unused]] void* arg )
	{
		while ( true )
		{
			char conMessage[1024] = { 0, };
			gets_s( conMessage, sizeof( conMessage ) );

			if ( strlen( conMessage ) == 0 )
			{
				continue;
			}

			engine::GetConsoleMessageExecutor( ).AppendCommand( conMessage );
		}

		return 0;
	}
}

namespace logic
{
	DebugConsole::DebugConsole()
	{
		if ( AllocConsole() )
		{
			/* Disable the close button on the console window.
			 * If the program is terminated via the console window, it cannot go through the normal shutdown process.
			 * Note: This method cannot prevent Alt + F4 from closing the window.
			 * Therefore, I plan to create a console window using ImGUI in the future. */
			if( HWND hwnd = GetConsoleWindow() )
			{
				if( HMENU hMenu = GetSystemMenu( hwnd, FALSE ) )
				{
					EnableMenuItem( hMenu, SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
				}
			}

			freopen_s( &m_pConOut, "CONOUT$", "wt", stdout );
			freopen_s( &m_pConIn, "CONIN$", "r", stdin );
			m_thread = (HANDLE)_beginthreadex( nullptr, 0, asyncInputFunc, nullptr, 0, nullptr );
		}
	}

	DebugConsole::~DebugConsole()
	{
		CloseHandle( m_thread );

		FreeConsole();
		::fclose( m_pConOut );
		::fclose( m_pConIn );
	}
}
