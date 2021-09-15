#include "stdafx.h"
#include "Window.h"

bool Window::Run( CWindowSetup& setup, WNDPROC wndProc )
{
	WNDCLASSEXA wndclsex = setup.GetWndClass( );
	wndclsex.lpszClassName = m_wndTitle.c_str( );
	wndclsex.lpfnWndProc = wndProc;

	if ( RegisterClassExA( &wndclsex ) )
	{
		RECT rc = setup.GetScreenArea( );
		m_width = rc.right - rc.left;
		m_height = rc.bottom - rc.top;

		AdjustWindowRect( &rc, m_style, false );

		m_hwnd = CreateWindowA( m_wndTitle.c_str( ),
								m_wndTitle.c_str( ),
								m_style,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								rc.right - rc.left,
								rc.bottom - rc.top,
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

Window::Window( const std::string& title, uint32 style ) noexcept : m_wndTitle( title ), m_style( style )
{
}

