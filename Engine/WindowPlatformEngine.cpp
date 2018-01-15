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

	m_logicDll = LoadLibrary( _T( "../bin/Logic.dll" ) );

	if ( m_logicDll == nullptr )
	{
		return false;
	}

	using CreateGameLogicFunc = Owner<ILogic*> (*)( );
	CreateGameLogicFunc CreateGameLogic = reinterpret_cast<CreateGameLogicFunc>( GetProcAddress( m_logicDll, "CreateGameLogic" ) );

	if ( CreateGameLogic == nullptr )
	{
		return false;
	}

	m_logic.reset( CreateGameLogic( ) );

	if ( m_logic )
	{
		m_isAvailable = m_logic->Initialize( platform );
	}

	m_platform = &platform;

	return m_isAvailable;
}

void WindowPlatformEngine::ShutDown( )
{
	m_logic = nullptr;
	m_isAvailable = false;
	FreeLibrary( m_logicDll );
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

LRESULT WindowPlatformEngine::MsgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	case WM_ACTIVATE:
		if ( LOWORD( wParam ) == WA_INACTIVE )
		{
			m_logic->Pause( );
		}
		else
		{
			m_logic->Resume( );
		}
		return 0;
	case WM_ENTERSIZEMOVE:
		m_logic->Pause( );
		m_resizing = true;
		return 0;
	case WM_EXITSIZEMOVE:
		m_logic->Resume( );
		m_resizing = false;
		m_logic->AppSizeChanged( *m_platform );
		return 0;
	case WM_SIZE:
		m_platform->UpdateSize( LOWORD( lParam ), HIWORD( lParam ) );

		if ( wParam == SIZE_MINIMIZED )
		{
			m_logic->Pause( );
			m_maximized = false;
			m_minimized = true;
		}
		else if ( wParam == SIZE_MAXIMIZED )
		{
			m_maximized = true;
			m_minimized = false;
		}
		else if ( wParam == SIZE_RESTORED )
		{
			if ( m_minimized )
			{
				m_logic->Resume( );
			}
			else if ( m_maximized )
			{
				m_logic->Resume( );
			}
			else if ( m_resizing )
			{

			}
			else
			{
				m_logic->AppSizeChanged( *m_platform );
			}
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
}

Owner<IEngine*> CreatePlatformEngine( SUPPORT_PLATFORM::Window ) { return new WindowPlatformEngine( ); }