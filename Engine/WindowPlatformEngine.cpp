#include "stdafx.h"
#include "WindowPlatformEngine.h"

#include "../Logic/ILogic.h"
#include "../shared/IPlatform.h"
#include "../shared/Util.h"

#include <Windows.h>

bool WindowPlatformEngine::BootUp( IPlatform& platform )
{
	if ( !m_inputConvertor.Initialize( ) )
	{
		return false;
	}

	HMODULE logicDll = LoadLibrary( _T( "../bin/Logic.dll" ) );

	if ( logicDll == nullptr )
	{
		return false;
	}

	using CreateGameLogicFunc = Owner<ILogic*> (*)( );
	CreateGameLogicFunc CreateGameLogic = reinterpret_cast<CreateGameLogicFunc>( GetProcAddress( logicDll, "CreateGameLogic" ) );

	if ( CreateGameLogic == nullptr )
	{
		return false;
	}

	m_logic.reset( CreateGameLogic( ) );

	if ( m_logic )
	{
		m_isAvailable = m_logic->Initialize( platform );
	}

	return m_isAvailable;
}

void WindowPlatformEngine::ShutDown( )
{
	m_logic = nullptr;
	m_isAvailable = false;
}

void WindowPlatformEngine::Run( )
{
	while ( IsAvailable() )
	{
		ProcessInput( );
		m_logic->Update( );
	}
}

void WindowPlatformEngine::ProcessInput( )
{
	MSG msg;
	if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	{
		m_isAvailable = msg.message != WM_QUIT;

		if ( m_inputConvertor.ProcessInput( *m_logic, msg ) )
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

Owner<IEngine*> CreatePlatformEngine( SUPPORT_PLATFORM::Window ) { return new WindowPlatformEngine( ); }