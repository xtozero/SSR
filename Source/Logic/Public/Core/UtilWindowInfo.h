#pragma once

#include "SizedTypes.h"

namespace logic
{
	class UtilWindowInfo
	{
	public:
		static UtilWindowInfo& GetInstance()
		{
			static UtilWindowInfo windowInfo;
			return windowInfo;
		}

		uint32 GetWidth() const { return m_wndRect.right; }
		uint32 GetHeight() const { return m_wndRect.bottom; }
		const RECT& GetRect() const { return m_wndRect; }

		void SetRect( uint32 right, uint32 bottom, uint32 left = 0, uint32 top = 0 )
		{
			m_wndRect.left = left;
			m_wndRect.top = top;
			m_wndRect.right = right;
			m_wndRect.bottom = bottom;
		}

	private:
		RECT m_wndRect = {};
	};
}
