#pragma once

#include "Math/Matrix.h"
#include "Math/Matrix3X3.h"
#include "Math/Plane.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"

#include <cmath>
#include <utility>
#include <windows.h>

// http://www.songho.ca/math/plane/plane.html
// Intersection of 3 Planes
inline bool PlanesIntersection( const Plane& p0, const Plane& p1, const Plane& p2, Point& point )
{
	using namespace DirectX;

	Vector n0( p0.x, p0.y, p0.z );
	Vector n1( p1.x, p1.y, p1.z );
	Vector n2( p2.x, p2.y, p2.z );

	auto n0xn1 = n0 ^ n1;
	auto n1xn2 = n1 ^ n2;
	auto n2xn0 = n2 ^ n0;

	float detA = n0 | n1xn2;

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

inline XMMatrix XM_CALLCONV lerp( const XMMatrix& a, const XMMatrix& b, float prop )
{
	assert( prop <= 1.f );
	XMMatrix result;
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

inline Matrix3X3 MakeSkewSymmetric( const Vector& v )
{
	return Matrix3X3(
		0.f, -v.z, v.y,
		v.z, 0.f, -v.x,
		-v.y, v.x, 0.f );
}

inline Matrix3X3 MakeInertiaTensorCoeffs( float ix, float iy, float iz, float ixy = 0, float iyz = 0, float ixz = 0 )
{
	return Matrix3X3(
		ix, -ixy, -ixz,
		-ixy, iy, -iyz,
		-ixz, -iyz, iz );
}

inline Matrix3X3 MakeBlockInertiaTensor( const Vector& halfSizes, float mass )
{
	if ( mass == FLT_MAX )
	{
		return Matrix3X3( 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f );
	}

	Vector squares = halfSizes * halfSizes;
	return MakeInertiaTensorCoeffs( 0.3f * mass * ( squares.y + squares.z ),
		0.3f * mass * ( squares.x + squares.z ),
		0.3f * mass * ( squares.x + squares.y ) );
}

inline Matrix3X3 MakeSphereInertiaTensor( const float radius, float mass )
{
	if ( mass == FLT_MAX )
	{
		return Matrix3X3( 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f );
	}

	float coeff = 0.4f * mass * radius * radius;
	return MakeInertiaTensorCoeffs( coeff, coeff, coeff );
}

inline Vector SphericalToCartesian( float inclination, float azimuth )
{
	float sinTheta = sin( inclination );
	float cosTheta = cos( inclination );
	float sinPhi = sin( azimuth );
	float cosPhi = cos( azimuth );

	return Vector( sinTheta * sinPhi, cosTheta, -sinTheta * cosPhi );
}

inline std::pair<float, float> CartesianToSpherical( const Vector& cartesian )
{
	using namespace DirectX;

	float theta = acos( cartesian.y / cartesian.Length() );
	float phi = atan2( cartesian.x, -cartesian.z );

	return std::make_pair( theta, phi );
}