#pragma once

#include "CXMFloat.h"

#include <cmath>
#include <windows.h>

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

inline DirectX::XMMATRIX XM_CALLCONV lerp( DirectX::FXMMATRIX a, DirectX::CXMMATRIX b, float prop )
{
	assert( prop <= 1.f );
	DirectX::XMMATRIX result;
	float omp = 1.f - prop;

	__m128 p = _mm_set1_ps( prop );
	__m128 o = _mm_set1_ps( omp );

	for ( int i = 0; i < 4; ++i )
	{
		result.r[i] = _mm_add_ps(
			_mm_mul_ps( a.r[i], o ), 
			_mm_mul_ps( b.r[i], p ) 
		);
	}

	return result;
}

inline CXMFLOAT3X3 MakeSkewSymmetric( const CXMFLOAT3& v )
{
	return CXMFLOAT3X3(
		0.f, -v.z, v.y,
		v.z, 0.f, -v.x,
		-v.y, v.x, 0.f );
}

inline CXMFLOAT3X3 MakeInertiaTensorCoeffs( float ix, float iy, float iz, float ixy = 0, float iyz = 0, float ixz = 0 )
{
	return CXMFLOAT3X3(
		ix, -ixy, -ixz,
		-ixy, iy, -iyz,
		-ixz, -iyz, iz );
}

inline CXMFLOAT3X3 MakeBlockInertiaTensor( const CXMFLOAT3& halfSizes, float mass )
{
	if ( mass == FLT_MAX )
	{
		return CXMFLOAT3X3( 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f );
	}

	CXMFLOAT3 squares = halfSizes * halfSizes;
	return MakeInertiaTensorCoeffs( 0.3f * mass * ( squares.y + squares.z ),
		0.3f * mass * ( squares.x + squares.z ),
		0.3f * mass * ( squares.x + squares.y ) );
}

inline CXMFLOAT3X3 MakeSphereInertiaTensor( const float radius, float mass )
{
	if ( mass == FLT_MAX )
	{
		return CXMFLOAT3X3( 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f );
	}

	float coeff = 0.4f * mass * radius * radius;
	return MakeInertiaTensorCoeffs( coeff, coeff, coeff );
}
