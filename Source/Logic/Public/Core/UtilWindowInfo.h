#pragma once

class CUtilWindowInfo
{
public:
	static CUtilWindowInfo& GetInstance( )
	{
		static CUtilWindowInfo windowInfo;
		return windowInfo;
	}

	unsigned int GetWidth( ) { return m_wndRect.right; }
	unsigned int GetHeight( ) { return m_wndRect.bottom; }
	const RECT GetRect( ) { return m_wndRect; }

	void SetRect( unsigned int right, unsigned int bottom, unsigned int left = 0, unsigned int top = 0 )
	{
		m_wndRect.left = left;
		m_wndRect.top = top;
		m_wndRect.right = right;
		m_wndRect.bottom = bottom;
	}

private:
	RECT m_wndRect = {};
};