#pragma once

#include <string>
#include <Windows.h>

#include "common.h"

class ENGINE_DLL CWindowSetup
{
public:
	RECT GetScreenArea( ) const noexcept { return { 0, 0, m_width, m_height }; }
	const WNDCLASSEX& GetWndClass( ) const noexcept { return m_wndClass; }
	const HINSTANCE GethInstance( ) const noexcept { return m_wndClass.hInstance; }

	CWindowSetup( HINSTANCE hInstance, UINT width, UINT height ) noexcept : m_width( width ), m_height( height )
	{
		m_wndClass.style = CS_HREDRAW | CS_VREDRAW;
		m_wndClass.lpfnWndProc = nullptr;
		m_wndClass.cbClsExtra = 0;
		m_wndClass.cbWndExtra = 0;
		m_wndClass.hInstance = hInstance;
		m_wndClass.hIcon = nullptr;
		m_wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
		m_wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		m_wndClass.lpszMenuName = nullptr;
		m_wndClass.lpszClassName = nullptr;
		m_wndClass.hIconSm = nullptr;
	}

private:
	WNDCLASSEX m_wndClass = { sizeof( WNDCLASSEX ), };
	long m_width = 0;
	long m_height = 0;
};

class ENGINE_DLL Window
{
public:
	bool Run( CWindowSetup& setup, WNDPROC wndProc );
	HWND GetHwnd( ) const noexcept { return m_hwnd; }

	Window( const String& title ) noexcept;
	~Window( );

private:
	String m_wndTitle = _T( "default window" );
	DWORD m_style = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	HWND m_hwnd = nullptr;
};

