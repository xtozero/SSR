#pragma once

#include "Math/Vector2.h"

#include <windows.h>

struct Rect
{
	Point2 m_leftTop;
	Point2 m_rightBottom;

	Vector2 GetWidthHeight() const { return m_rightBottom - m_leftTop; }

	Rect() = default;
	Rect( float left, float top, float right, float bottom )
	{
		m_leftTop.x = left;
		m_leftTop.y = top;
		m_rightBottom.x = right;
		m_rightBottom.y = bottom;
	}
	Rect( const Point2& leftTop, const Point2& rightBottom )
	{
		m_leftTop = leftTop;
		m_rightBottom = rightBottom;
	}
	Rect& operator=( const RECT& rect )
	{
		m_leftTop.x = static_cast<float>( rect.left );
		m_leftTop.y = static_cast<float>( rect.top );
		m_rightBottom.x = static_cast<float>( rect.right );
		m_rightBottom.y = static_cast<float>( rect.bottom );
		return *this;
	}

	friend bool operator==( const Rect& lhs, const Rect& rhs )
	{
		return lhs.m_leftTop == rhs.m_leftTop && lhs.m_rightBottom == rhs.m_rightBottom;
	}
};