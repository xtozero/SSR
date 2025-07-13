#include "InterfaceFactories.h"
#include "LibraryTool/Common.h"
#include "Platform/WindowPlatformEngine.h"
#include "ScreenSpaceReflection.h"
#include "SizedTypes.h"
#include "Window.h"

#include <memory>

using ::app::WindowSetup;
using ::app::Window;
using ::engine::IEngine;
using ::engine::WindowPlatformEngine;

LRESULT CALLBACK WndProc( HWND, uint32, WPARAM, LPARAM );

constexpr int32 FRAME_BUFFER_WIDTH = 1024;
constexpr int32 FRAME_BUFFER_HEIGHT = 768;

WindowPlatformEngine* GEngine = nullptr;

int32 APIENTRY WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR lpszCmdParam, _In_ int32 )
{
	WindowSetup setup( hInstance, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT );
	Window mainWindow( "Screen Space Reflection" );

	if ( !mainWindow.Run( setup, WndProc ) )
	{
		return false;
	}

	HMODULE engineDll = LoadModule( "Engine.dll" );
	if ( engineDll == nullptr )
	{
		return false;
	}

	GEngine = static_cast<WindowPlatformEngine*>( GetInterface<IEngine>() );
	if ( GEngine == nullptr )
	{
		return false;
	}

	if ( !GEngine->BootUp( mainWindow, lpszCmdParam ) )
	{
		return false;
	}

	GEngine->Run();

	ShutdownModule( engineDll );

	return 0;
}

LRESULT CALLBACK WndProc( HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam )
{
	if ( GEngine )
	{
		return GEngine->MsgProc( hWnd, message, wParam, lParam );
	}

	return DefWindowProcA( hWnd, message, wParam, lParam );
}