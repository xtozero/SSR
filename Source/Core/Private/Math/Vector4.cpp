#include "Vector4.h"

#include "Vector.h"

using ::DirectX::XMVectorGetX;
using ::DirectX::XMVector3Cross;
using ::DirectX::XMVector4Dot;
using ::DirectX::XMVector4Equal;
using ::DirectX::XMVector4Length;
using ::DirectX::XMVector4LengthSq;
using ::DirectX::XMVector4Normalize;

float Vector4::Length() const
{
	return XMVectorGetX( XMVector4Length( XMVector( *this ) ) );
}

float Vector4::LengthSqrt() const
{
	return XMVectorGetX( XMVector4LengthSq( XMVector( *this ) ) );
}

XMVector Vector4::GetAbs() const
{
	return XMVector( *this ).GetAbs();
}

XMVector Vector4::GetNormalized() const
{
	return XMVector4Normalize( XMVector( *this ) );
}

Vector4::Vector4( const XMVector& v )
{
	XMStoreFloat4( this, v.m_xmmReg );
}

Vector4::Vector4( const Vector& v ) : XMFLOAT4( v.x, v.y, v.z, 0 )
{
}

Vector4& Vector4::operator=( const XMVector& v )
{
	XMStoreFloat4( this, v.m_xmmReg );
	return *this;
}

float& Vector4::operator[]( size_t idx )
{
	switch (idx)
	{
	case 0:
		return x;
		break;
	case 1:
		return y;
		break;
	case 2:
		return z;
		break;
	case 3:
		return w;
		break;
	default:
		__debugbreak();
		break;
	}

	return x;
}

float Vector4::operator[]( size_t idx ) const
{
	switch (idx)
	{
	case 0:
		return x;
		break;
	case 1:
		return y;
		break;
	case 2:
		return z;
		break;
	case 3:
		return w;
		break;
	default:
		__debugbreak();
		break;
	}

	return x;
}

XMVector operator+( const Vector4& lhs, const Vector4& rhs )
{
	return XMVector( lhs ) + XMVector( rhs );
}

XMVector operator+( const XMVector& lhs, const Vector4& rhs )
{
	return lhs + XMVector( rhs );
}

XMVector operator+( const Vector4& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) + rhs;
}

XMVector operator-( const Vector4& lhs, const Vector4& rhs )
{
	return XMVector( lhs ) - XMVector( rhs );
}

XMVector operator-( const XMVector& lhs, const Vector4& rhs )
{
	return lhs - XMVector( rhs );
}

XMVector operator-( const Vector4& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) - rhs;
}

XMVector operator*( const Vector4& lhs, const Vector4& rhs )
{
	return XMVector( lhs ) * XMVector( rhs );
}

XMVector operator*( const XMVector& lhs, const Vector4& rhs )
{
	return lhs * XMVector( rhs );
}

XMVector operator*( const Vector4& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) * rhs;
}

XMVector operator/( const Vector4& lhs, const Vector4& rhs )
{
	return XMVector( lhs ) / XMVector( rhs );
}

XMVector operator/( const XMVector& lhs, const Vector4& rhs )
{
	return lhs / XMVector( rhs );
}

XMVector operator/( const Vector4& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) / rhs;
}

XMVector operator^( const Vector4& lhs, const Vector4& rhs )
{
	return XMVector3Cross( XMVector( lhs ), XMVector( rhs ) );
}

XMVector operator^( const XMVector& lhs, const Vector4& rhs )
{
	return XMVector3Cross( lhs, XMVector( rhs ) );
}

XMVector operator^( const Vector4& lhs, const XMVector& rhs )
{
	return XMVector3Cross( XMVector( lhs ), rhs );
}

float operator|( const Vector4& lhs, const Vector4& rhs )
{
	return XMVectorGetX( XMVector4Dot( XMVector( lhs ), XMVector( rhs ) ) );
}

float operator|( const XMVector& lhs, const Vector4& rhs )
{
	return XMVectorGetX( XMVector4Dot( lhs, XMVector( rhs ) ) );
}

float operator|( const Vector4& lhs, const XMVector& rhs )
{
	return XMVectorGetX( XMVector4Dot( XMVector( lhs ), rhs ) );
}

bool operator==( const Vector4& lhs, const Vector4& rhs )
{
	return XMVector4Equal( XMVector( lhs ), XMVector( rhs ) );
}

bool operator==( const XMVector& lhs, const Vector4& rhs )
{
	return XMVector4Equal( lhs, XMVector( rhs ) );
}

bool operator==( const Vector4& lhs, const XMVector& rhs )
{
	return XMVector4Equal( XMVector( lhs ), rhs );
}

XMVector operator*( const Vector4& v, float s )
{
	return XMVector( v ) * s;
}

XMVector operator*( float s, const Vector4& v )
{
	return s * XMVector( v );
}

XMVector operator/( const Vector4& v, float s )
{
	return XMVector( v ) / s;
}

Vector4& operator+=( Vector4& lhs, const Vector4& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

XMVector& operator+=( XMVector& lhs, const Vector4& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

Vector4& operator+=( Vector4& lhs, const XMVector& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

Vector4& operator-=( Vector4& lhs, const Vector4& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

XMVector& operator-=( XMVector& lhs, const Vector4& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

Vector4& operator-=( Vector4& lhs, const XMVector& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

Vector4& operator*=( Vector4& lhs, const Vector4& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

XMVector& operator*=( XMVector& lhs, const Vector4& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Vector4& operator*=( Vector4& lhs, const XMVector& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Vector4& operator/=( Vector4& lhs, const Vector4& rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

XMVector& operator/=( XMVector& lhs, const Vector4& rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

Vector4& operator/=( Vector4& lhs, const XMVector& rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

Vector4& operator*=( Vector4& v, float s )
{
	v = v * s;
	return v;
}

Vector4& operator/=( Vector4& v, float s )
{
	v = v / s;
	return v;
}

XMVector operator+( const Vector4& v )
{
	return XMVector( v );
}

XMVector operator-( const Vector4& v )
{
	return -XMVector( v );
}

const Vector4 Vector4::OneVector( 1.f, 1.f, 1.f, 1.f );
const Vector4 Vector4::ZeroVector( 0.f, 0.f, 0.f, 0.f );
