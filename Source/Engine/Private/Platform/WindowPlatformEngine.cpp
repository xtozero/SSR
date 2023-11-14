#include "stdafx.h"
#include "WindowPlatformEngine.h"

#include "AppConfig/AppConfig.h"
#include "common.h"
#include "CommandLine.h"
#include "Config/DefaultAppConfig.h"
#include "Core/IEditor.h"
#include "Core/ILogic.h"
#include "FileSystem.h"
#include "GuideTypes.h"
#include "InterfaceFactories.h"
#include "IPlatform.h"
#include "TaskScheduler.h"

#include <Windows.h>

namespace engine
{
	bool WindowPlatformEngine::BootUp( IPlatform& platform, char* argv )
	{
		if ( GetInterface<CommandLine>() == nullptr )
		{
			return false;
		}

		GetInterface<CommandLine>()->Parse( argv );

		if ( !m_inputConvertor.Initialize() )
		{
			return false;
		}

		m_fileSystem = GetInterface<IFileSystem>();
		if ( m_fileSystem == nullptr )
		{
			return false;
		}

		m_taskScheduler = GetInterface<ITaskScheduler>();
		if ( m_taskScheduler == nullptr )
		{
			return false;
		}

		auto appConfig = GetInterface<IAppConfig>();
		if ( appConfig == nullptr )
		{
			return false;
		}

		std::atomic<int32> iniWorkInProgress;
		appConfig->BootUp( iniWorkInProgress );

		while ( iniWorkInProgress != 0 )
		{
			GetInterface<ITaskScheduler>()->ProcessThisThreadTask();
		}

		std::pair<int32, int32> clientSize = DefaultApp::GetClientSize();
		platform.Resize( clientSize.first, clientSize.second );

		m_logicDll = LoadModule( DefaultApp::IsEditor() ? "Editor.dll" : "Logic.dll" );
		if ( m_logicDll == nullptr )
		{
			return false;
		}

		m_logic = DefaultApp::IsEditor() ? GetInterface<editor::IEditor>() : GetInterface<logic::ILogic>();

		if ( m_logic )
		{
			m_isAvailable = m_logic->BootUp( platform );
		}

		m_platform = &platform;

		return m_isAvailable;
	}

	void WindowPlatformEngine::Shutdown()
	{
		m_logic = nullptr;
		m_isAvailable = false;

		GetInterface<IAppConfig>()->SaveConfigToFile();

		ShutdownModule( m_logicDll );
	}

	void WindowPlatformEngine::Run()
	{
		while ( IsAvailable() )
		{
			ProcessInput();
			m_taskScheduler->ProcessThisThreadTask();
			m_logic->Update();
		}
	}

	void WindowPlatformEngine::ProcessInput()
	{
		MSG msg;
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			m_isAvailable = msg.message != WM_QUIT;

			TranslateMessage( &msg );
			DispatchMessage( &msg );

			m_inputConvertor.ProcessInput( *m_logic, msg );
		}
	}

	LRESULT WindowPlatformEngine::MsgProc( HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam )
	{
		switch ( message )
		{
		case WM_ACTIVATE:
			if ( m_logic )
			{
				if ( LOWORD( wParam ) == WA_INACTIVE )
				{
					m_logic->Pause();
				}
				else
				{
					m_logic->Resume();
				}
			}
			return 0;
		case WM_ENTERSIZEMOVE:
			m_logic->Pause();
			m_resizing = true;
			return 0;
		case WM_EXITSIZEMOVE:
			m_logic->Resume();
			m_resizing = false;
			m_logic->AppSizeChanged( *m_platform );
			return 0;
		case WM_SIZE:
		{
			if ( ( m_platform == nullptr ) || ( m_logic == nullptr ) )
			{
				return 0;
			}

			int32 width = LOWORD( lParam );
			int32 height = HIWORD( lParam );

			m_platform->UpdateSize( width, height );
			DefaultApp::SetClientSize( { width, height } );

			if ( wParam == SIZE_MINIMIZED )
			{
				m_logic->Pause();
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
					m_logic->Resume();
				}
				else if ( m_maximized )
				{
					m_logic->Resume();
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
		}
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
		}
	}

	WindowPlatformEngine::~WindowPlatformEngine()
	{
		Shutdown();
	}

	Owner<IEngine*> CreatePlatformEngine()
	{
		return new WindowPlatformEngine();
	}

	void DestroyPlatformEngine( Owner<IEngine*> pEngine )
	{
		delete pEngine;
	}
}
