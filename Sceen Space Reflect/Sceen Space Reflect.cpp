#include "stdafx.h"
#include "Sceen Space Reflect.h"
#include "..\Logic\GameLogic.h"
#include "..\Engine\Window.h"

constexpr int FRAME_BUFFER_WIDTH = 1024;
constexpr int FRAME_BUFFER_HEIGHT = 768;

#pragma comment( lib, "../lib/Logic.lib")
#pragma comment( lib, "../lib/Engine.lib")


LRESULT CALLBACK	WndProc ( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK	About ( HWND, UINT, WPARAM, LPARAM );

int APIENTRY _tWinMain ( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE , _In_ LPTSTR , _In_ int )
{
	CWindowSetup setup( hInstance, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT );
	Window mainWindow( _T( "Screen Space Reflection" ) );

	if ( !mainWindow.Run( setup, WndProc ) )
	{
		return FALSE;	
	}

	CGameLogic gGameLogic;
	gGameLogic.Initialize( mainWindow.GetHwnd( ), FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT );

	MSG msg;
	while ( 1 )
	{
		if ( PeekMessage ( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if ( msg.message == WM_QUIT )
			{
				break;
			}
			
			if ( gGameLogic.HandleWindowMessage( msg ) )
			{
				//Do Nothing
			}
			else
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
		
		gGameLogic.UpdateLogic( );
	}

	return (int)msg.wParam;
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