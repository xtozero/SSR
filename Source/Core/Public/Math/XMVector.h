#pragma once

#include <DirectXMath.h>

struct Plane;
struct Vector;
struct Vector2;
struct Vector4;

struct XMVector
{
	float Length() const;
	float LengthSqrt() const;
	XMVector GetAbs() const;
	XMVector GetNormalized() const;

	XMVector( DirectX::XMVECTOR v ) : m_xmmReg( v ) { }
	XMVector( const Plane& p );
	XMVector( const Vector& v );
	XMVector( const Vector2& v );
	XMVector( const Vector4& v );

	friend XMVector operator+( const XMVector& v );
	friend XMVector operator-( const XMVector& v );

	friend XMVector& operator+=( XMVector& lhs, const XMVector& rhs );
	friend XMVector& operator-=( XMVector& lhs, const XMVector& rhs );
	friend XMVector& operator*=( XMVector& lhs, const XMVector& rhs );
	friend XMVector& operator/=( XMVector& lhs, const XMVector& rhs );

	friend XMVector& operator*=( XMVector& v, float s );
	friend XMVector& operator/=( XMVector& v, float s );

	friend XMVector operator+( const XMVector& lhs, const XMVector& rhs );
	friend XMVector operator-( const XMVector& lhs, const XMVector& rhs );

	friend XMVector operator*( const XMVector& lhs, const XMVector& rhs );
	friend XMVector operator*( const XMVector& v, float s );
	friend XMVector operator*( float s, const XMVector& v );

	friend XMVector operator/( const XMVector& lhs, const XMVector& rhs );
	friend XMVector operator/( const XMVector& v, float s );

	friend XMVector operator^( const XMVector& lhs, const XMVector& rhs );

	friend float operator|( const XMVector& lhs, const XMVector& rhs );

	friend bool operator==( const XMVector& lhs, const XMVector& rhs );

	operator DirectX::XMVECTOR() const
	{
		return m_xmmReg;
	}

	DirectX::XMVECTOR m_xmmReg;
};