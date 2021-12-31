#include "Quaternion.h"

#include "Vector.h"

#include <memory>

using namespace DirectX;

float Quaternion::Length() const
{
	return XMVectorGetX( XMVector( *this ) );
}

XMVector Quaternion::GetNormalized() const
{
	return XMQuaternionNormalize( XMVector( *this ) );
}

XMVector Quaternion::Inverse() const
{
	return XMQuaternionInverse( XMVector( *this ) );
}

Quaternion::Quaternion( float pitch, float yaw, float roll )
{
	std::construct_at( reinterpret_cast<Quaternion*>( this ), XMQuaternionRotationRollPitchYaw( pitch, yaw, roll ) );
}

Quaternion::Quaternion( const XMVector& v )
{
	XMStoreFloat4( this, v.m_xmmReg );
}

Quaternion& Quaternion::operator=( const XMVector& v )
{
	XMStoreFloat4( this, v.m_xmmReg );
	return *this;
}

float& Quaternion::operator[]( size_t idx )
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

float Quaternion::operator[]( size_t idx ) const
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

XMVector operator+( const Quaternion& lhs, const Quaternion& rhs )
{
	return XMVector( lhs ) + XMVector( rhs );
}

XMVector operator+( const XMVector& lhs, const Quaternion& rhs )
{
	return lhs + XMVector( rhs );
}

XMVector operator+( const Quaternion& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) + rhs;
}

XMVector operator-( const Quaternion& lhs, const Quaternion& rhs )
{
	return XMVector( lhs ) - XMVector( rhs );
}

XMVector operator-( const XMVector& lhs, const Quaternion& rhs )
{
	return lhs - XMVector( rhs );
}

XMVector operator-( const Quaternion& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) - rhs;
}

XMVector operator*( const Quaternion& lhs, const Quaternion& rhs )
{
	return XMQuaternionMultiply( XMVector( rhs ), XMVector( lhs ) );
}

XMVector operator*( const XMVector& lhs, const Quaternion& rhs )
{
	return XMQuaternionMultiply( XMVector( rhs ), lhs );
}

XMVector operator*( const Quaternion& lhs, const XMVector& rhs )
{
	return XMQuaternionMultiply( rhs, XMVector( lhs ) );
}

float operator|( const Quaternion& lhs, const Quaternion& rhs )
{
	return XMVectorGetX( XMQuaternionDot( XMVector( lhs ), XMVector( rhs ) ) );
}

float operator|( const XMVector& lhs, const Quaternion& rhs )
{
	return XMVectorGetX( XMQuaternionDot( lhs, XMVector( rhs ) ) );
}

float operator|( const Quaternion& lhs, const XMVector& rhs )
{
	return XMVectorGetX( XMQuaternionDot( XMVector( lhs ), rhs ) );
}

bool operator==( const Quaternion& lhs, const Quaternion& rhs )
{
	return XMQuaternionEqual( XMVector( lhs ), XMVector( rhs ) );
}

bool operator==( const XMVector& lhs, const Quaternion& rhs )
{
	return XMQuaternionEqual( lhs, XMVector( rhs ) );
}

bool operator==( const Quaternion& lhs, const XMVector& rhs )
{
	return XMQuaternionEqual( XMVector( lhs ), rhs );
}

XMVector operator*( const Quaternion& v, float s )
{
	return XMVector( v ) * s;
}

XMVector operator*( float s, const Quaternion& v )
{
	return s * XMVector( v );
}

XMVector operator/( const Quaternion& v, float s )
{
	return XMVector( v ) / s;
}

Quaternion& operator+=( Quaternion& lhs, const Quaternion& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

XMVector& operator+=( XMVector& lhs, const Quaternion& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

Quaternion& operator+=( Quaternion& lhs, const XMVector& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

Quaternion& operator-=( Quaternion& lhs, const Quaternion& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

XMVector& operator-=( XMVector& lhs, const Quaternion& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

Quaternion& operator-=( Quaternion& lhs, const XMVector& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

Quaternion& operator*=( Quaternion& lhs, const Quaternion& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

XMVector& operator*=( XMVector& lhs, const Quaternion& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Quaternion& operator*=( Quaternion& lhs, const XMVector& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Quaternion& operator*=( Quaternion& v, float s )
{
	v = v * s;
	return v;
}

Quaternion& operator/=( Quaternion& v, float s )
{
	v = v / s;
	return v;
}

const Quaternion Quaternion::Identity( 0.f, 0.f, 0.f, 1.f );