#include "stdafx.h"
#include "common.h"
#include "Core/IEngine.h"
#include "InterfaceFactories.h"
#include "ScreenSpaceReflection.h"
#include "SizedTypes.h"
#include "Window.h"

#include <memory>

using ::app::CWindowSetup;
using ::app::Window;
using ::engine::IEngine;

LRESULT CALLBACK WndProc( HWND, uint32, WPARAM, LPARAM );

constexpr int32 FRAME_BUFFER_WIDTH = 1024;
constexpr int32 FRAME_BUFFER_HEIGHT = 768;

IEngine* g_engine = nullptr;

int32 APIENTRY WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR lpszCmdParam, _In_ int32 )
{
	CWindowSetup setup( hInstance, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT );
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

	g_engine = GetInterface<IEngine>();
	if ( g_engine == nullptr )
	{
		return false;
	}

	if ( !g_engine->BootUp( mainWindow, lpszCmdParam ) )
	{
		return false;
	}

	g_engine->Run();

	ShutdownModule( engineDll );

	return 0;
}

LRESULT CALLBACK WndProc( HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam )
{
	if ( g_engine )
	{
		return g_engine->MsgProc( hWnd, message, wParam, lParam );
	}

	return DefWindowProcA( hWnd, message, wParam, lParam );
}