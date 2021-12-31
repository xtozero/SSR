#pragma once

#include "Math/XMVector.h"

#include <DirectXMath.h>

struct Vector4;

struct Vector : public DirectX::XMFLOAT3
{
public:
	float Length() const;
	float LengthSqrt() const;
	XMVector GetAbs() const;
	XMVector GetNormalized() const;

	using DirectX::XMFLOAT3::XMFLOAT3;
	Vector( const XMVector& v );
	Vector( const Vector4& v );
	Vector() : DirectX::XMFLOAT3() {}

	Vector& operator=( const XMVector& v );

	float& operator[]( size_t idx );
	float operator[]( size_t idx ) const;

	friend XMVector operator+( const Vector& lhs, const Vector& rhs );
	friend XMVector operator+( const XMVector& lhs, const Vector& rhs );
	friend XMVector operator+( const Vector& lhs, const XMVector& rhs );
	
	friend XMVector operator-( const Vector& lhs, const Vector& rhs );
	friend XMVector operator-( const XMVector& lhs, const Vector& rhs );
	friend XMVector operator-( const Vector& lhs, const XMVector& rhs );
	
	friend XMVector operator*( const Vector& lhs, const Vector& rhs );
	friend XMVector operator*( const XMVector& lhs, const Vector& rhs );
	friend XMVector operator*( const Vector& lhs, const XMVector& rhs );

	friend XMVector operator/( const Vector& lhs, const Vector& rhs );
	friend XMVector operator/( const XMVector& lhs, const Vector& rhs );
	friend XMVector operator/( const Vector& lhs, const XMVector& rhs );

	friend XMVector operator^( const Vector& lhs, const Vector& rhs );
	friend XMVector operator^( const XMVector& lhs, const Vector& rhs );
	friend XMVector operator^( const Vector& lhs, const XMVector& rhs );

	friend float operator|( const Vector& lhs, const Vector& rhs );
	friend float operator|( const XMVector& lhs, const Vector& rhs );
	friend float operator|( const Vector& lhs, const XMVector& rhs );

	friend bool operator==( const Vector& lhs, const Vector& rhs );
	friend bool operator==( const XMVector& lhs, const Vector& rhs );
	friend bool operator==( const Vector& lhs, const XMVector& rhs );

	friend XMVector operator*( const Vector& v, float s );
	friend XMVector operator*( float s, const Vector& v );

	friend XMVector operator/( const Vector& v, float s );

	friend Vector& operator+=( Vector& lhs, const Vector& rhs );
	friend XMVector& operator+=( XMVector& lhs, const Vector& rhs );
	friend Vector& operator+=( Vector& lhs, const XMVector& rhs );

	friend Vector& operator-=( Vector& lhs, const Vector& rhs );
	friend XMVector& operator-=( XMVector& lhs, const Vector& rhs );
	friend Vector& operator-=( Vector& lhs, const XMVector& rhs );

	friend Vector& operator*=( Vector& lhs, const Vector& rhs );
	friend XMVector& operator*=( XMVector& lhs, const Vector& rhs );
	friend Vector& operator*=( Vector& lhs, const XMVector& rhs );

	friend Vector& operator/=( Vector& lhs, const Vector& rhs );
	friend XMVector& operator/=( XMVector& lhs, const Vector& rhs );
	friend Vector& operator/=( Vector& lhs, const XMVector& rhs );

	friend Vector& operator*=( Vector& v, float s );
	friend Vector& operator/=( Vector& v, float s );

	friend XMVector operator+( const Vector& v );
	friend XMVector operator-( const Vector& v );

	operator XMVector() { return DirectX::XMLoadFloat3( this ); }
	operator const XMVector() const { return DirectX::XMLoadFloat3( this ); }

	static const Vector ZAxisVector;
	static const Vector XAxisVector;
	static const Vector YAxisVector;
	static const Vector ForwardVector;
	static const Vector RightVector;
	static const Vector UpVector;
	static const Vector OneVector;
	static const Vector ZeroVector;
};

using Point = Vector;