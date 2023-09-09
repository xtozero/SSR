#pragma once

#include "Math/XMVector.h"

struct Vector;

struct Vector4 : public DirectX::XMFLOAT4
{
public:
	float Length() const;
	float LengthSqrt() const;
	XMVector GetAbs() const;
	XMVector GetNormalized() const;

	using ::DirectX::XMFLOAT4::XMFLOAT4;
	Vector4( const XMVector& v );
	Vector4( const Vector& v );
	Vector4() : DirectX::XMFLOAT4() {}

	Vector4& operator=( const XMVector& v );

	float& operator[]( size_t idx );
	float operator[]( size_t idx ) const;

	friend XMVector operator+( const Vector4& lhs, const Vector4& rhs );
	friend XMVector operator+( const XMVector& lhs, const Vector4& rhs );
	friend XMVector operator+( const Vector4& lhs, const XMVector& rhs );

	friend XMVector operator-( const Vector4& lhs, const Vector4& rhs );
	friend XMVector operator-( const XMVector& lhs, const Vector4& rhs );
	friend XMVector operator-( const Vector4& lhs, const XMVector& rhs );

	friend XMVector operator*( const Vector4& lhs, const Vector4& rhs );
	friend XMVector operator*( const XMVector& lhs, const Vector4& rhs );
	friend XMVector operator*( const Vector4& lhs, const XMVector& rhs );

	friend XMVector operator/( const Vector4& lhs, const Vector4& rhs );
	friend XMVector operator/( const XMVector& lhs, const Vector4& rhs );
	friend XMVector operator/( const Vector4& lhs, const XMVector& rhs );

	friend XMVector operator^( const Vector4& lhs, const Vector4& rhs );
	friend XMVector operator^( const XMVector& lhs, const Vector4& rhs );
	friend XMVector operator^( const Vector4& lhs, const XMVector& rhs );

	friend float operator|( const Vector4& lhs, const Vector4& rhs );
	friend float operator|( const XMVector& lhs, const Vector4& rhs );
	friend float operator|( const Vector4& lhs, const XMVector& rhs );

	friend bool operator==( const Vector4& lhs, const Vector4& rhs );
	friend bool operator==( const XMVector& lhs, const Vector4& rhs );
	friend bool operator==( const Vector4& lhs, const XMVector& rhs );

	friend XMVector operator*( const Vector4& v, float s );
	friend XMVector operator*( float s, const Vector4& v );

	friend XMVector operator/( const Vector4& v, float s );

	friend Vector4& operator+=( Vector4& lhs, const Vector4& rhs );
	friend XMVector& operator+=( XMVector& lhs, const Vector4& rhs );
	friend Vector4& operator+=( Vector4& lhs, const XMVector& rhs );

	friend Vector4& operator-=( Vector4& lhs, const Vector4& rhs );
	friend XMVector& operator-=( XMVector& lhs, const Vector4& rhs );
	friend Vector4& operator-=( Vector4& lhs, const XMVector& rhs );

	friend Vector4& operator*=( Vector4& lhs, const Vector4& rhs );
	friend XMVector& operator*=( XMVector& lhs, const Vector4& rhs );
	friend Vector4& operator*=( Vector4& lhs, const XMVector& rhs );

	friend Vector4& operator/=( Vector4& lhs, const Vector4& rhs );
	friend XMVector& operator/=( XMVector& lhs, const Vector4& rhs );
	friend Vector4& operator/=( Vector4& lhs, const XMVector& rhs );

	friend Vector4& operator*=( Vector4& v, float s );
	friend Vector4& operator/=( Vector4& v, float s );

	friend XMVector operator+( const Vector4& v );
	friend XMVector operator-( const Vector4& v );

	operator XMVector() { return DirectX::XMLoadFloat4( this ); }
	operator const XMVector() const { return DirectX::XMLoadFloat4( this ); }

	static const Vector4 OneVector;
	static const Vector4 ZeroVector;
};
