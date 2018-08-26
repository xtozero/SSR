#pragma once

#include "CXMFloat.h"

// http://www.songho.ca/math/plane/plane.html
// Intersection of 3 Planes
inline bool PlanesIntersection( const CXMFLOAT4& p0, const CXMFLOAT4& p1, const CXMFLOAT4& p2, CXMFLOAT3& point )
{
	using namespace DirectX;

	CXMFLOAT3 n0( p0.x, p0.y, p0.z );
	CXMFLOAT3 n1( p1.x, p1.y, p1.z );
	CXMFLOAT3 n2( p2.x, p2.y, p2.z );

	XMVECTOR n0xn1 = XMVector3Cross( n0, n1 );
	XMVECTOR n1xn2 = XMVector3Cross( n1, n2 );
	XMVECTOR n2xn0 = XMVector3Cross( n2, n0 );

	float detA = XMVectorGetX( XMVector3Dot( n0, n1xn2 ) );

	switch ( fpclassify( detA ) )
	{
	case FP_ZERO:
	case FP_INFINITE:
	case FP_NAN:
		return false;
	}

	float rcpDetA = 1.f / detA;

	point = -( p0.w * n1xn2 + p1.w * n2xn0 + p2.w * n0xn1 ) * rcpDetA;
	return true;
}

struct Rect
{
	CXMFLOAT2 m_leftTop;
	CXMFLOAT2 m_rightBottom;
	
	CXMFLOAT2 GetWidthHeight( ) const { return m_rightBottom - m_leftTop; }

	Rect( ) = default;
	Rect( float left, float top, float right, float bottom )
	{
		m_leftTop.x = left;
		m_leftTop.y = top;
		m_rightBottom.x = right;
		m_rightBottom.y = bottom;
	}
	Rect( const CXMFLOAT2& leftTop, const CXMFLOAT2& rightBottom )
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
};

inline bool operator==( const Rect& lhs, const Rect& rhs )
{
	return lhs.m_leftTop == rhs.m_leftTop && lhs.m_rightBottom == rhs.m_rightBottom;
}