#pragma once

#include "common.h"
#include "Platform/IPlatform.h"
#include "SizedTypes.h"

#include <string>
#include <Windows.h>

class CWindowSetup
{
public:
	RECT GetScreenArea( ) const noexcept { return{ 0, 0, m_width, m_height }; }
	const WNDCLASSEXA& GetWndClass( ) const noexcept { return m_wndClass; }
	const HINSTANCE GethInstance( ) const noexcept { return m_wndClass.hInstance; }

	CWindowSetup( HINSTANCE hInstance, int32 width, int32 height ) noexcept : m_width( width ), m_height( height )
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
	WNDCLASSEXA m_wndClass = { sizeof( WNDCLASSEXA ), };
	int32 m_width = 0;
	int32 m_height = 0;
};

class Window : public  IPlatform
{
public:
	virtual std::pair<uint32, uint32> GetSize( ) const noexcept override { return { m_width, m_height }; };
	virtual void UpdateSize( uint32 width, uint32 height )
	{
		m_width = width;
		m_height = height;
	}

	bool Run( CWindowSetup& setup, WNDPROC wndProc );
	HWND GetHwnd( ) const noexcept { return m_hwnd; }

	Window( const std::string& title, uint32 style = WS_OVERLAPPEDWINDOW ) noexcept;

private:
	virtual void* GetRawHandleImple( ) const noexcept override
	{
		return m_hwnd;
	}

	std::string m_wndTitle = "default window";
	uint32 m_style = 0;
	HWND m_hwnd = nullptr;

	uint32 m_width = 0;
	uint32 m_height = 0;
};

