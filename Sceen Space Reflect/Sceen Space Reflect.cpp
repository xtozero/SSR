#include "stdafx.h"
#include "Sceen Space Reflect.h"
#include "../Engine/IEngine.h"
#include "Window.h"

#include <memory>

constexpr int FRAME_BUFFER_WIDTH = 1024;
constexpr int FRAME_BUFFER_HEIGHT = 768;

LRESULT CALLBACK	WndProc ( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK	About ( HWND, UINT, WPARAM, LPARAM );

int APIENTRY _tWinMain ( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE , _In_ LPTSTR , _In_ int )
{
	CWindowSetup setup( hInstance, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT );
	Window mainWindow( _T( "Screen Space Reflection" ) );

	if ( !mainWindow.Run( setup, WndProc ) )
	{
		return false;
	}

	HMODULE engineDll = LoadLibrary( _T( "../bin/Engine.dll" ) );

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

	std::unique_ptr<IEngine> engine( CreatePlatformEngine( SUPPORT_PLATFORM::Window() ) );
	if ( engine == nullptr )
	{
		return false;
	}

	if ( !engine->BootUp( mainWindow ) )
	{
		return false;
	}

	engine->Run( );
	
	engine->ShutDown( );
	FreeLibrary( engineDll );

	return 0;
}

LRESULT CALLBACK WndProc ( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch ( message )
	{
	case WM_PAINT:
		hdc = BeginPaint ( hWnd, &ps );
		EndPaint ( hWnd, &ps );
		break;
	case WM_DESTROY:
		PostQuitMessage ( 0 );
		break;
	default:
		return DefWindowProc ( hWnd, message, wParam, lParam );
	}
	return 0;
}