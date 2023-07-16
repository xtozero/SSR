#pragma once

template <typename T>
struct RectangleArea
{
	T m_left;
	T m_top;
	T m_right;
	T m_bottom;

	bool Valid() const
	{
		return m_left < m_right && m_top < m_bottom;
	}
};

template<typename T>
struct CubeArea
{
	T m_left;
	T m_top;
	T m_front;
	T m_right;
	T m_bottom;
	T m_back;
};