#include "stdafx.h"
#include "Platform/WindowPlatformEngine.h"
#include "Sceen Space Reflect.h"
#include "Window.h"

#include <memory>

constexpr int FRAME_BUFFER_WIDTH = 1024;
constexpr int FRAME_BUFFER_HEIGHT = 768;

LRESULT CALLBACK	WndProc ( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK	About ( HWND, UINT, WPARAM, LPARAM );

WindowPlatformEngine* g_engine = nullptr;

int APIENTRY _tWinMain ( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE , _In_ LPTSTR , _In_ int )
{
	CWindowSetup setup( hInstance, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT );
	Window mainWindow( _T( "Screen Space Reflection" ) );

	if ( !mainWindow.Run( setup, WndProc ) )
	{
		return false;
	}

	HMODULE engineDll = LoadLibrary( _T( "./Binaries/Engine.dll" ) );

	if ( engineDll == nullptr )
	{
		return false;
	}

	using CreateEngineFunc = Owner<IEngine*> (*)( SUPPORT_PLATFORM::Window );
	CreateEngineFunc CreatePlatformEngine = reinterpret_cast<CreateEngineFunc>( GetProcAddress( engineDll, "CreatePlatformEngine" ) );

	if ( CreatePlatformEngine == nullptr )
	{
		return false;
	}

	g_engine = dynamic_cast<WindowPlatformEngine*>( CreatePlatformEngine( SUPPORT_PLATFORM::Window() ) );
	if ( g_engine == nullptr )
	{
		return false;
	}

	if ( !g_engine->BootUp( mainWindow ) )
	{
		return false;
	}

	g_engine->Run( );
	
	g_engine->ShutDown( );
	FreeLibrary( engineDll );

	return 0;
}

LRESULT CALLBACK WndProc ( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if ( g_engine )
	{
		return g_engine->MsgProc( hWnd, message, wParam, lParam );
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}