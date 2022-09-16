#include "XMVector.h"

#include "Plane.h"
#include "Vector.h"
#include "Vector2.h"
#include "Vector4.h"

using DirectX::operator+;
using DirectX::operator-;
using DirectX::operator*;
using DirectX::operator/;
using DirectX::operator+=;
using DirectX::operator-=;
using DirectX::operator*=;
using DirectX::operator/=;
using DirectX::XMFLOAT4;
using DirectX::XMStoreFloat4;
using DirectX::XMVectorAbs;
using DirectX::XMVectorEqual;
using DirectX::XMVectorGetX;
using DirectX::XMVector3Cross;
using DirectX::XMVector4Dot;
using DirectX::XMVector4Length;
using DirectX::XMVector4LengthSq;
using DirectX::XMVector4Normalize;

float XMVector::Length() const
{
	return XMVectorGetX( XMVector4Length( m_xmmReg ) );
}

float XMVector::LengthSqrt() const
{
	return XMVectorGetX( XMVector4LengthSq( m_xmmReg ) );
}

XMVector XMVector::GetAbs() const
{
	return XMVectorAbs( m_xmmReg );
}

XMVector XMVector::GetNormalized() const
{
	return XMVector4Normalize( m_xmmReg );
}

XMVector::XMVector( const Plane& p )
{
	m_xmmReg = XMLoadFloat4( &p );
}

XMVector::XMVector( const Vector& v )
{
	m_xmmReg = XMLoadFloat3( &v );
}

XMVector::XMVector( const Vector2& v )
{
	m_xmmReg = XMLoadFloat2( &v );
}

XMVector::XMVector( const Vector4& v )
{
	m_xmmReg = XMLoadFloat4( &v );
}

XMVector operator+( const XMVector& v )
{
	return XMVector( v.m_xmmReg );
}

XMVector operator-( const XMVector& v )
{
	return XMVector( -v.m_xmmReg );
}

XMVector& operator+=( XMVector& lhs, const XMVector& rhs )
{
	lhs.m_xmmReg += rhs.m_xmmReg;
	return lhs;
}

XMVector& operator-=( XMVector& lhs, const XMVector& rhs )
{
	lhs.m_xmmReg -= rhs.m_xmmReg;
	return lhs;
}

XMVector& operator*=( XMVector& lhs, const XMVector& rhs )
{
	lhs.m_xmmReg *= rhs.m_xmmReg;
	return lhs;
}

XMVector& operator/=( XMVector& lhs, const XMVector& rhs )
{
	lhs.m_xmmReg /= rhs.m_xmmReg;
	return lhs;
}

XMVector& operator*=( XMVector& v, float s )
{
	v.m_xmmReg *= s;
	return v;
}

XMVector& operator/=( XMVector& v, float s )
{
	v.m_xmmReg /= s;
	return v;
}

XMVector operator+( const XMVector& lhs, const XMVector& rhs )
{
	return XMVector( lhs.m_xmmReg + rhs.m_xmmReg );
}

XMVector operator-( const XMVector& lhs, const XMVector& rhs )
{
	return XMVector( lhs.m_xmmReg - rhs.m_xmmReg );
}

XMVector operator*( const XMVector& lhs, const XMVector& rhs )
{
	return XMVector( lhs.m_xmmReg * rhs.m_xmmReg );
}

XMVector operator*( const XMVector& v, float s )
{
	return XMVector( v.m_xmmReg * s );
}

XMVector operator*( float s, const XMVector& v )
{
	return XMVector( s * v.m_xmmReg );
}

XMVector operator/( const XMVector& lhs, const XMVector& rhs )
{
	return XMVector( lhs.m_xmmReg / rhs.m_xmmReg );
}

XMVector operator/( const XMVector& v, float s )
{
	return XMVector( v.m_xmmReg / s );
}

XMVector operator^( const XMVector& lhs, const XMVector& rhs )
{
	return XMVector3Cross( lhs, rhs );
}

float operator|( const XMVector& lhs, const XMVector& rhs )
{
	return XMVectorGetX( XMVector4Dot( lhs, rhs ) );
}

bool operator==( const XMVector& lhs, const XMVector& rhs )
{
	XMFLOAT4 equal;
	XMStoreFloat4( &equal, XMVectorEqual( lhs, rhs ) );
	return equal.x > 0.f && equal.y > 0.f && equal.z > 0.f && equal.w > 0.f;
}
