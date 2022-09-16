#include "Vector.h"

#include "Vector4.h"

using DirectX::XMVectorGetX;
using DirectX::XMVector3Cross;
using DirectX::XMVector3Dot;
using DirectX::XMVector3Equal;
using DirectX::XMVector3Length;
using DirectX::XMVector3LengthSq;
using DirectX::XMVector3Normalize;

float Vector::Length() const
{
	return XMVectorGetX( XMVector3Length( XMVector( *this ) ) );
}

float Vector::LengthSqrt() const
{
	return XMVectorGetX( XMVector3LengthSq( XMVector( *this ) ) );
}

XMVector Vector::GetAbs() const
{
	return XMVector( *this ).GetAbs();
}

XMVector Vector::GetNormalized() const
{
	return XMVector3Normalize( XMVector( *this ) );
}

Vector::Vector( const XMVector& v )
{
	XMStoreFloat3( this, v.m_xmmReg );
}

Vector::Vector( const Vector4& v ) : XMFLOAT3( v.x, v.y, v.z )
{
}

Vector& Vector::operator=( const XMVector& v )
{
	XMStoreFloat3( this, v.m_xmmReg );
	return *this;
}

float& Vector::operator[]( size_t idx )
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
	default:
		__debugbreak();
		break;
	}

	return x;
}

float Vector::operator[]( size_t idx ) const
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
	default:
		__debugbreak();
		break;
	}

	return x;
}

XMVector operator+( const Vector& lhs, const Vector& rhs )
{
	return XMVector( lhs ) + XMVector( rhs );
}

XMVector operator+( const XMVector& lhs, const Vector& rhs )
{
	return lhs + XMVector( rhs );
}

XMVector operator+( const Vector& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) + rhs;
}

XMVector operator-( const Vector& lhs, const Vector& rhs )
{
	return XMVector( lhs ) - XMVector( rhs );
}

XMVector operator-( const XMVector& lhs, const Vector& rhs )
{
	return lhs - XMVector( rhs );
}

XMVector operator-( const Vector& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) - rhs;
}

XMVector operator*( const Vector& lhs, const Vector& rhs )
{
	return XMVector( lhs ) * XMVector( rhs );
}

XMVector operator*( const XMVector& lhs, const Vector& rhs )
{
	return lhs * XMVector( rhs );
}

XMVector operator*( const Vector& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) * rhs;
}

XMVector operator/( const Vector& lhs, const Vector& rhs )
{
	return XMVector( lhs ) / XMVector( rhs );
}

XMVector operator/( const XMVector& lhs, const Vector& rhs )
{
	return lhs / XMVector( rhs );
}

XMVector operator/( const Vector& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) / rhs;
}

XMVector operator^( const Vector& lhs, const Vector& rhs )
{
	return XMVector3Cross( XMVector( lhs ), XMVector( rhs ) );
}

XMVector operator^( const XMVector& lhs, const Vector& rhs )
{
	return XMVector3Cross( lhs, XMVector( rhs ) );
}

XMVector operator^( const Vector& lhs, const XMVector& rhs )
{
	return XMVector3Cross( XMVector( lhs ), rhs );
}

float operator|( const Vector& lhs, const Vector& rhs )
{
	return XMVectorGetX( XMVector3Dot( XMVector( lhs ), XMVector( rhs ) ) );
}

float operator|( const XMVector& lhs, const Vector& rhs )
{
	return XMVectorGetX( XMVector3Dot( lhs, XMVector( rhs ) ) );
}

float operator|( const Vector& lhs, const XMVector& rhs )
{
	return XMVectorGetX( XMVector3Dot( XMVector( lhs ), rhs ) );
}

bool operator==( const Vector& lhs, const Vector& rhs )
{
	return XMVector3Equal( XMVector( lhs ), XMVector( rhs ) );
}

bool operator==( const XMVector& lhs, const Vector& rhs )
{
	return XMVector3Equal( lhs, XMVector( rhs ) );
}

bool operator==( const Vector& lhs, const XMVector& rhs )
{
	return XMVector3Equal( XMVector( lhs ), rhs );
}

XMVector operator*( const Vector& v, float s )
{
	return XMVector( v ) * s;
}

XMVector operator*( float s, const Vector& v )
{
	return s * XMVector( v );
}

XMVector operator/( const Vector& v, float s )
{
	return XMVector( v ) / s;
}

Vector& operator+=( Vector& lhs, const Vector& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

XMVector& operator+=( XMVector& lhs, const Vector& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

Vector& operator+=( Vector& lhs, const XMVector& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

Vector& operator-=( Vector& lhs, const Vector& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

XMVector& operator-=( XMVector& lhs, const Vector& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

Vector& operator-=( Vector& lhs, const XMVector& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

Vector& operator*=( Vector& lhs, const Vector& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

XMVector& operator*=( XMVector& lhs, const Vector& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Vector& operator*=( Vector& lhs, const XMVector& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Vector& operator/=( Vector& lhs, const Vector& rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

XMVector& operator/=( XMVector& lhs, const Vector& rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

Vector& operator/=( Vector& lhs, const XMVector& rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

Vector& operator*=( Vector& v, float s )
{
	v = v * s;
	return v;
}

Vector& operator/=( Vector& v, float s )
{
	v = v / s;
	return v;
}

XMVector operator+( const Vector& v )
{
	return XMVector( v );
}

XMVector operator-( const Vector& v )
{
	return -XMVector( v );
}

const Vector Vector::ZAxisVector( 0.f, 0.f, 1.f );
const Vector Vector::XAxisVector( 1.f, 0.f, 0.f );
const Vector Vector::YAxisVector( 0.f, 1.f, 0.f );
const Vector Vector::ForwardVector( 0.f, 0.f, 1.f );
const Vector Vector::RightVector( 1.f, 0.f, 0.f );
const Vector Vector::UpVector( 0.f, 1.f, 0.f );
const Vector Vector::OneVector( 1.f, 1.0f, 1.f );
const Vector Vector::ZeroVector( 0.f, 0.f, 0.f );
