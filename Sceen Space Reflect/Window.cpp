#include "stdafx.h"
#include "Window.h"

bool Window::Run( CWindowSetup& setup, WNDPROC wndProc )
{
	WNDCLASSEX wndclsex = setup.GetWndClass( );
	wndclsex.lpszClassName = m_wndTitle.c_str( );
	wndclsex.lpfnWndProc = wndProc;

	if ( RegisterClassEx( &wndclsex ) )
	{
		RECT rc = setup.GetScreenArea( );
		AdjustWindowRect( &rc, m_style, false );

		m_width = rc.right - rc.left;
		m_height = rc.bottom - rc.top;

		m_hwnd = CreateWindow( m_wndTitle.c_str( ),
								m_wndTitle.c_str( ),
								m_style,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								m_width,
								m_height,
								nullptr,
								nullptr,
								setup.GethInstance( ),
								nullptr );

		if ( !m_hwnd )
		{
			return false;
		}

		ShowWindow( m_hwnd, SW_SHOWNORMAL );
		UpdateWindow( m_hwnd );

		return true;
	}

	return false;
}

Window::Window( const String& title ) noexcept : m_wndTitle( title )
{
}

Window::~Window( )
{
}
