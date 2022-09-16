#include "Plane.h"

#include "Matrix.h"
#include "Vector4.h"

using DirectX::XMPlaneDot;
using DirectX::XMPlaneDotCoord;
using DirectX::XMPlaneDotNormal;
using DirectX::XMPlaneNormalize;
using DirectX::XMVectorGetX;

Vector Plane::GetNormal() const
{
	return Vector( x, y, z );
}

XMVector Plane::GetNormalized() const
{
	return XMPlaneNormalize( XMVector( *this ) );
}

XMVector Plane::TransformBy( const Matrix& m ) const
{
	return XMPlaneTransform( XMVector( *this ), XMMatrix( m ) );
}

float Plane::PlaneDot( const Vector& v ) const
{
	return XMVectorGetX( XMPlaneDotCoord( XMVector( *this ), XMVector( v ) ) );
}

float Plane::PlaneDot( const XMVector& v ) const
{
	return XMVectorGetX( XMPlaneDotCoord( XMVector( *this ), v ) );
}

float Plane::PlaneDotNormal( const Vector& v ) const
{
	return XMVectorGetX( XMPlaneDotNormal( XMVector( *this ), XMVector( v ) ) );
}

float Plane::PlaneDotNormal( const XMVector& v ) const
{
	return XMVectorGetX( XMPlaneDotNormal( XMVector( *this ), v ) );
}

Plane::Plane( const XMVector& v )
{
	XMStoreFloat4( this, v.m_xmmReg );
}

Plane::Plane( const Vector4& v ) : XMFLOAT4( v.x, v.y, v.z, v.w )
{
}

XMVector operator+( const Plane& lhs, const Plane& rhs )
{
	return XMVector( lhs ) + XMVector( rhs );
}

XMVector operator+( const XMVector& lhs, const Plane& rhs )
{
	return lhs + XMVector( rhs );
}

XMVector operator+( const Plane& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) + rhs;
}

XMVector operator-( const Plane& lhs, const Plane& rhs )
{
	return XMVector( lhs ) - XMVector( rhs );
}

XMVector operator-( const XMVector& lhs, const Plane& rhs )
{
	return lhs - XMVector( rhs );
}

XMVector operator-( const Plane& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) - rhs;
}

XMVector operator*( const Plane& lhs, const Plane& rhs )
{
	return XMVector( lhs ) * XMVector( rhs );
}

XMVector operator*( const XMVector& lhs, const Plane& rhs )
{
	return lhs * XMVector( rhs );
}

XMVector operator*( const Plane& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) * rhs;
}

XMVector operator/( const Plane& lhs, const Plane& rhs )
{
	return XMVector( lhs ) / XMVector( rhs );
}

XMVector operator/( const XMVector& lhs, const Plane& rhs )
{
	return lhs / XMVector( rhs );
}

XMVector operator/( const Plane& lhs, const XMVector& rhs )
{
	return XMVector( lhs ) / rhs;
}

float operator|( const Plane& lhs, const Plane& rhs )
{
	return XMVectorGetX( XMPlaneDot( XMVector( lhs ), XMVector( rhs ) ) );
}

float operator|( const XMVector& lhs, const Plane& rhs )
{
	return XMVectorGetX( XMPlaneDot( lhs, XMVector( rhs ) ) );
}

float operator|( const Plane& lhs, const XMVector& rhs )
{
	return XMVectorGetX( XMPlaneDot( XMVector( lhs ), rhs ) );
}

XMVector operator*( const Plane& v, float s )
{
	return XMVector( v ) * s;
}

XMVector operator*( float s, const Plane& v )
{
	return s * XMVector( v );
}

XMVector operator/( const Plane& v, float s )
{
	return XMVector( v ) / s;
}

Plane& operator+=( Plane& lhs, const Plane& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

XMVector& operator+=( XMVector& lhs, const Plane& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

Plane& operator+=( Plane& lhs, const XMVector& rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

Plane& operator-=( Plane& lhs, const Plane& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

XMVector& operator-=( XMVector& lhs, const Plane& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

Plane& operator-=( Plane& lhs, const XMVector& rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

Plane& operator*=( Plane& lhs, const Plane& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

XMVector& operator*=( XMVector& lhs, const Plane& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Plane& operator*=( Plane& lhs, const XMVector& rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Plane& operator/=( Plane& lhs, const Plane& rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

XMVector& operator/=( XMVector& lhs, const Plane& rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

Plane& operator/=( Plane& lhs, const XMVector& rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

Plane& operator*=( Plane& v, float s )
{
	v = v * s;
	return v;
}

Plane& operator/=( Plane& v, float s )
{
	v = v / s;
	return v;
}
