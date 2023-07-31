#include "stdafx.h"
#include "Window.h"

std::pair<uint32, uint32> Window::GetSize() const noexcept
{
	return { m_width, m_height };
}

void Window::UpdateSize( uint32 width, uint32 height )
{
	m_width = width;
	m_height = height;
}

void Window::Resize( uint32 width, uint32 height )
{
	if ( !m_hwnd )
	{
		return;
	}

	RECT rc = {
		.left = 0,
		.top = 0,
		.right = static_cast<int32>( width ),
		.bottom = static_cast<int32>( height )
	};
	AdjustWindowRect( &rc, m_style, false );
	SetWindowPos( m_hwnd, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE );
	UpdateSize( width, height );
}

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

		m_hwnd = CreateWindowExA( WS_EX_NOREDIRECTIONBITMAP,
								m_wndTitle.c_str( ),
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

