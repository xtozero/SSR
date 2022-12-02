#pragma once

#include "Math/Matrix.h"
#include "Math/Matrix3X3.h"
#include "Math/Plane.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "SizedTypes.h"

#include <cmath>
#include <utility>
#include <windows.h>

// http://www.songho.ca/math/plane/plane.html
// Intersection of 3 Planes
inline bool PlanesIntersection( const Plane& p0, const Plane& p1, const Plane& p2, Point& point )
{
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

	for ( uint32 i = 0; i < 4; ++i )
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
	float theta = acos( cartesian.y / cartesian.Length() );
	float phi = atan2( cartesian.x, -cartesian.z );

	return std::make_pair( theta, phi );
}

template <typename T>
T CalcAlignment( T value, T alignment )
{
	T mask = alignment - 1;
	return ( value + mask ) & ~mask;
}

template uint32 CalcAlignment<uint32>( uint32 value, uint32 alignment );
template uint64 CalcAlignment<uint64>( uint64 value, uint64 alignment );
template int32 CalcAlignment<int32>( int32 value, int32 alignment );
template int64 CalcAlignment<int64>( int64 value, int64 alignment );