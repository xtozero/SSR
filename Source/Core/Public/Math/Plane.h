#pragma once

#include "Math/Vector.h"
#include "Math/XMVector.h"

#include <DirectXMath.h>

struct Matrix;
struct Vector4;

struct Plane : public DirectX::XMFLOAT4
{
public:
	Vector GetNormal() const;
	XMVector GetNormalized() const;
	XMVector TransformBy( const Matrix& m ) const;
	float PlaneDot( const Vector& v ) const;
	float PlaneDot( const XMVector& v ) const;
	float PlaneDotNormal( const Vector& v ) const;
	float PlaneDotNormal( const XMVector& v ) const;

	using DirectX::XMFLOAT4::XMFLOAT4;
	Plane( const XMVector& v );
	Plane( const Vector4& v );
	Plane() : DirectX::XMFLOAT4() {}

	friend XMVector operator+( const Plane& lhs, const Plane& rhs );
	friend XMVector operator+( const XMVector& lhs, const Plane& rhs );
	friend XMVector operator+( const Plane& lhs, const XMVector& rhs );

	friend XMVector operator-( const Plane& lhs, const Plane& rhs );
	friend XMVector operator-( const XMVector& lhs, const Plane& rhs );
	friend XMVector operator-( const Plane& lhs, const XMVector& rhs );

	friend XMVector operator*( const Plane& lhs, const Plane& rhs );
	friend XMVector operator*( const XMVector& lhs, const Plane& rhs );
	friend XMVector operator*( const Plane& lhs, const XMVector& rhs );

	friend XMVector operator/( const Plane& lhs, const Plane& rhs );
	friend XMVector operator/( const XMVector& lhs, const Plane& rhs );
	friend XMVector operator/( const Plane& lhs, const XMVector& rhs );

	friend float operator|( const Plane& lhs, const Plane& rhs );
	friend float operator|( const XMVector& lhs, const Plane& rhs );
	friend float operator|( const Plane& lhs, const XMVector& rhs );

	friend XMVector operator*( const Plane& v, float s );
	friend XMVector operator*( float s, const Plane& v );

	friend XMVector operator/( const Plane& v, float s );

	friend Plane& operator+=( Plane& lhs, const Plane& rhs );
	friend XMVector& operator+=( XMVector& lhs, const Plane& rhs );
	friend Plane& operator+=( Plane& lhs, const XMVector& rhs );

	friend Plane& operator-=( Plane& lhs, const Plane& rhs );
	friend XMVector& operator-=( XMVector& lhs, const Plane& rhs );
	friend Plane& operator-=( Plane& lhs, const XMVector& rhs );

	friend Plane& operator*=( Plane& lhs, const Plane& rhs );
	friend XMVector& operator*=( XMVector& lhs, const Plane& rhs );
	friend Plane& operator*=( Plane& lhs, const XMVector& rhs );

	friend Plane& operator/=( Plane& lhs, const Plane& rhs );
	friend XMVector& operator/=( XMVector& lhs, const Plane& rhs );
	friend Plane& operator/=( Plane& lhs, const XMVector& rhs );

	friend Plane& operator*=( Plane& v, float s );
	friend Plane& operator/=( Plane& v, float s );
};