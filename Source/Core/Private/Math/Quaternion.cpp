#include "Quaternion.h"

#include "Rotator.h"
#include "Vector.h"

#include <cmath>
#include <memory>
#include <numbers>

using ::DirectX::XMConvertToDegrees;
using ::DirectX::XMQuaternionDot;
using ::DirectX::XMQuaternionEqual;
using ::DirectX::XMQuaternionInverse;
using ::DirectX::XMQuaternionMultiply;
using ::DirectX::XMQuaternionNormalize;
using ::DirectX::XMQuaternionRotationRollPitchYaw;
using ::DirectX::XMVector4LessOrEqual;
using ::DirectX::XMVectorAbs;
using ::DirectX::XMVectorGetX;

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

bool Quaternion::Equals( const Quaternion& other, float tolerance ) const
{
	XMVector toleranceVec = Vector4( tolerance, tolerance, tolerance, tolerance );
	XMVector rotationSub = XMVectorAbs( *this - other );
	XMVector rotationAdd = XMVectorAbs( *this + other );

	return XMVector4LessOrEqual( rotationSub, toleranceVec ) || XMVector4LessOrEqual( rotationAdd, toleranceVec );
}

Rotator Quaternion::ToRotator() const
{
	// from http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
	// heading -> yaw
	// attitude -> roll
	// bank -> pitch
	Quaternion q = GetNormalized();

	float pitch = 0.f;
	float yaw = 0.f;
	float roll = 0.f;

	float test = q.x * q.y + q.z * q.w;
	if ( test > 0.499f )
	{
		pitch = 0.f;
		yaw = 2.f * atan2( q.x, q.w );
		roll = std::numbers::pi_v<float> * 0.5f;
	}
	else if ( test < -0.499f )
	{
		pitch = 0.f;
		yaw = -2.f * atan2( q.x, q.w );
		roll = -std::numbers::pi_v<float> * 0.5f;
	}
	else
	{
		pitch = std::atan2( 2.f * ( q.x * q.w - q.y * q.z ), 1.f - 2.f * ( q.x * q.x + q.z * q.z ) );
		yaw = std::atan2( 2.f * ( q.y * q.w - q.x * q.z ) , 1.f - 2.f * ( q.y * q.y + q.z * q.z ) );
		roll = std::asin( 2.f * test );
	}

	return Rotator( XMConvertToDegrees( pitch ), XMConvertToDegrees( yaw ), XMConvertToDegrees( roll ) );
}

Quaternion::Quaternion( float pitch, float yaw, float roll )
{
	std::construct_at( reinterpret_cast<Quaternion*>( this ), XMQuaternionRotationRollPitchYaw( pitch, yaw, roll ) );
}

Quaternion::Quaternion( const XMVector& v )
{
	XMStoreFloat4( this, v.m_xmmReg );
}

Quaternion::Quaternion( const Matrix& m )
{
	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "TQuaternion Calculus and Fast Animation".

	const float tr = m.m[0][0] + m.m[1][1] + m.m[2][2];

	if ( tr > 0 )
	{
		float root = sqrtf( tr + 1.f );
		w = 0.5f * root;
		root = 0.5f / root;

		x = ( m.m[1][2] - m.m[2][1] ) * root;
		y = ( m.m[2][0] - m.m[0][2] ) * root;
		z = ( m.m[0][1] - m.m[1][0] ) * root;
	}
	else
	{
		uint32 i = 0;

		if ( m.m[1][1] > m.m[0][0] )
		{
			i = 1;
		}

		if ( m.m[2][2] > m.m[i][i] )
		{
			i = 2;
		}

		static constexpr uint32 nxt[3] = { 1, 2, 0 };
		const uint32 j = nxt[i];
		const uint32 k = nxt[j];

		float root = sqrtf( m.m[i][i] - m.m[j][j] - m.m[k][k] + 1 );

		float* qt[3] = { &x, &y, &z };
		*qt[i] = 0.5f * root;

		root = 0.5f / root;

		*qt[j] = ( m.m[i][j] - m.m[j][i] ) * root;
		*qt[k] = ( m.m[i][k] - m.m[k][i] ) * root;

		w = ( m.m[j][k] - m.m[k][j] ) * root;
	}
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