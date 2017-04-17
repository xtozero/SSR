#include "stdafx.h"
#include "WindowPlatformEngine.h"

#include "../Logic/ILogic.h"
#include "../shared/IPlatform.h"
#include "../shared/Util.h"

#include <Windows.h>

bool WindowPlatformEngine::BootUp( IPlatform& platform )
{
	m_logic.reset( CreateGameLogic( ) );

	if ( m_logic )
	{
		return m_logic->Initialize( platform );
	}

	return false;
}

void WindowPlatformEngine::ShutDown( )
{
	m_logic = nullptr;
}

void WindowPlatformEngine::ProcessInput( )
{
	MSG msg;
	while ( true )
	{
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if ( msg.message == WM_QUIT )
			{
				break;
			}

			FIX_ME( "여기서 입력 처리해야 함." );
			if ( false )
			{
				//Do Nothing
			}
			else
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
	}
}

void WindowPlatformEngine::Run( )
{
	ProcessInput( );
	m_logic->Update( );
}

Owner<IEngine*> CreatePlatformEngine( SUPPORT_PLATFORM::Window ) { return new WindowPlatformEngine( ); }