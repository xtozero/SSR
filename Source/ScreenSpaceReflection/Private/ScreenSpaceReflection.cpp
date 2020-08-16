#include "stdafx.h"
#include "common.h"
#include "Core/IEngine.h"
#include "Core/InterfaceFactories.h"
#include "ScreenSpaceReflection.h"
#include "Window.h"

#include <memory>

constexpr int FRAME_BUFFER_WIDTH = 1024;
constexpr int FRAME_BUFFER_HEIGHT = 768;

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

IEngine* g_engine = nullptr;

int APIENTRY WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int )
{
	CWindowSetup setup( hInstance, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT );
	Window mainWindow( "Screen Space Reflection" );

	if ( !mainWindow.Run( setup, WndProc ) )
	{
		return false;
	}

	HMODULE engineDll = LoadModule( "./Binaries/Engine.dll" );
	if ( engineDll == nullptr )
	{
		return false;
	}

	g_engine = GetInterface<IEngine>( );
	if ( g_engine == nullptr )
	{
		return false;
	}

	if ( !g_engine->BootUp( mainWindow ) )
	{
		return false;
	}

	g_engine->Run( );

	ShutdownModule( engineDll );

	return 0;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if ( g_engine )
	{
		return g_engine->MsgProc( hWnd, message, wParam, lParam );
	}

	return DefWindowProcA( hWnd, message, wParam, lParam );
}