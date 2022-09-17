#include "Vector2.h"

#include "Vector4.h"

using ::DirectX::XMVectorGetX;
using ::DirectX::XMVector2Cross;
using ::DirectX::XMVector2Dot;
using ::DirectX::XMVector2Equal;
using ::DirectX::XMVector2Length;
using ::DirectX::XMVector2LengthSq;
using ::DirectX::XMVector2Normalize;

float Vector2::Length() const
{
	return XMVectorGetX( XMVector2Length( XMVector( *this ) ) );
}

float Vector2::LengthSqrt() const
{
	return XMVectorGetX( XMVector2LengthSq( XMVector( *this ) ) );
}

XMVector Vector2::GetAbs() const
{
	return XMVector( *this ).GetAbs();
}

XMVector Vector2::GetNormalized() const
{
	return XMVector2Normalize( XMVector( *this ) );
}

Vector2::Vector2( const XMVector& v )
{
	XMStoreFloat2( this, v.m_xmmReg );
}

Vector2& Vector2::operator=( const XMVector& v )
{
	XMStoreFloat2( this, v.m_xmmReg );
	return *this;
}

float& Vector2::operator[]( size_t idx )
{
	switch (idx)
	{
	case 0:
		return x;
		break;
	case 1:
		return y;
		break;
	default:
		__debugbreak();
		break;
	}

	return x;
}

float Vector2::operator[]( size_t idx ) const
{
	switch (idx)
	{
	case 0:
		return x;
		break;
	case 1:
		return y;
		break;
	default:
		__debugbreak();
		break;
	}

	return x;
}

XMVector operator+( const Vector2& lhs, const Vector2& rhs )
{
	return XMVector( lhs ) + XMVector( rhs );
}

XMVector operator+( const XMVector& lhs, const Vector2& rhs )
{
	return lhs + XMVector( rhs );
}

XMVector operator+( const Vector2& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) + rhs;
}

XMVector operator-( const Vector2& lhs, const Vector2& rhs )
{
	return XMVector( lhs ) - XMVector( rhs );
}

XMVector operator-( const XMVector& lhs, const Vector2& rhs )
{
	return lhs - XMVector( rhs );
}

XMVector operator-( const Vector2& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) - rhs;
}

XMVector operator*( const Vector2& lhs, const Vector2& rhs )
{
	return XMVector( lhs ) * XMVector( rhs );
}

XMVector operator*( const XMVector& lhs, const Vector2& rhs )
{
	return lhs * XMVector( rhs );
}

XMVector operator*( const Vector2& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) * rhs;
}

XMVector operator/( const Vector2& lhs, const Vector2& rhs )
{
	return XMVector( lhs ) / XMVector( rhs );
}

XMVector operator/( const XMVector& lhs, const Vector2& rhs )
{
	return lhs / XMVector( rhs );
}

XMVector operator/( const Vector2& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) / rhs;
}

XMVector operator^( const Vector2& lhs, const Vector2& rhs )
{
	return XMVector2Cross( XMVector( lhs ), XMVector( rhs ) );
}

XMVector operator^( const XMVector& lhs, const Vector2& rhs )
{
	return XMVector2Cross( lhs, XMVector( rhs ) );
}

XMVector operator^( const Vector2& lhs, const XMVector& rhs )
{
	return XMVector2Cross( XMVector( lhs ), rhs );
}

float operator|( const Vector2& lhs, const Vector2& rhs )
{
	return XMVectorGetX( XMVector2Dot( XMVector( lhs ), XMVector( rhs ) ) );
}

float operator|( const XMVector& lhs, const Vector2& rhs )
{
	return XMVectorGetX( XMVector2Dot( lhs, XMVector( rhs ) ) );
}

float operator|( const Vector2& lhs, const XMVector& rhs )
{
	return XMVectorGetX( XMVector2Dot( XMVector( lhs ), rhs ) );
}

bool operator==( const Vector2& lhs, const Vector2& rhs )
{
	return XMVector2Equal( XMVector( lhs ), XMVector( rhs ) );
}

bool operator==( const XMVector& lhs, const Vector2& rhs )
{
	return XMVector2Equal( lhs, XMVector( rhs ) );
}

bool operator==( const Vector2& lhs, const XMVector& rhs )
{
	return XMVector2Equal( XMVector( lhs ), rhs );
}

XMVector operator*( const Vector2& v, float s )
{
	return XMVector( v ) * s;
}

XMVector operator*( float s, const Vector2& v )
{
	return s * XMVector( v );
}

XMVector operator/( const Vector2& v, float s )
{
	return XMVector( v ) / s;
}

Vector2& operator+=( Vector2& lhs, const Vector2& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

XMVector& operator+=( XMVector& lhs, const Vector2& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

Vector2& operator+=( Vector2& lhs, const XMVector& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

Vector2& operator-=( Vector2& lhs, const Vector2& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

XMVector& operator-=( XMVector& lhs, const Vector2& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

Vector2& operator-=( Vector2& lhs, const XMVector& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

Vector2& operator*=( Vector2& lhs, const Vector2& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

XMVector& operator*=( XMVector& lhs, const Vector2& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Vector2& operator*=( Vector2& lhs, const XMVector& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Vector2& operator/=( Vector2& lhs, const Vector2& rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

XMVector& operator/=( XMVector& lhs, const Vector2& rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

Vector2& operator/=( Vector2& lhs, const XMVector& rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

Vector2& operator*=( Vector2& v, float s )
{
	v = v * s;
	return v;
}

Vector2& operator/=( Vector2& v, float s )
{
	v = v / s;
	return v;
}

XMVector operator+( const Vector2& v )
{
	return XMVector( v );
}

XMVector operator-( const Vector2& v )
{
	return -XMVector( v );
}

const Vector2 Vector2::OneVector( 1.f, 1.0f );
const Vector2 Vector2::ZeroVector( 0.f, 0.f );