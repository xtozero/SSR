#pragma once

#include "Math/XMVector.h"

struct Vector2 : public DirectX::XMFLOAT2
{
public:
	float Length() const;
	float LengthSqrt() const;
	XMVector GetAbs() const;
	XMVector GetNormalized() const;

	using ::DirectX::XMFLOAT2::XMFLOAT2;
	Vector2( const XMVector& v );
	Vector2() : DirectX::XMFLOAT2() {}

	Vector2& operator=( const XMVector& v );

	float& operator[]( size_t idx );
	float operator[]( size_t idx ) const;

	friend XMVector operator+( const Vector2& lhs, const Vector2& rhs );
	friend XMVector operator+( const XMVector& lhs, const Vector2& rhs );
	friend XMVector operator+( const Vector2& lhs, const XMVector& rhs );
	
	friend XMVector operator-( const Vector2& lhs, const Vector2& rhs );
	friend XMVector operator-( const XMVector& lhs, const Vector2& rhs );
	friend XMVector operator-( const Vector2& lhs, const XMVector& rhs );
	
	friend XMVector operator*( const Vector2& lhs, const Vector2& rhs );
	friend XMVector operator*( const XMVector& lhs, const Vector2& rhs );
	friend XMVector operator*( const Vector2& lhs, const XMVector& rhs );

	friend XMVector operator/( const Vector2& lhs, const Vector2& rhs );
	friend XMVector operator/( const XMVector& lhs, const Vector2& rhs );
	friend XMVector operator/( const Vector2& lhs, const XMVector& rhs );

	friend XMVector operator^( const Vector2& lhs, const Vector2& rhs );
	friend XMVector operator^( const XMVector& lhs, const Vector2& rhs );
	friend XMVector operator^( const Vector2& lhs, const XMVector& rhs );

	friend float operator|( const Vector2& lhs, const Vector2& rhs );
	friend float operator|( const XMVector& lhs, const Vector2& rhs );
	friend float operator|( const Vector2& lhs, const XMVector& rhs );

	friend bool operator==( const Vector2& lhs, const Vector2& rhs );
	friend bool operator==( const XMVector& lhs, const Vector2& rhs );
	friend bool operator==( const Vector2& lhs, const XMVector& rhs );

	friend XMVector operator*( const Vector2& v, float s );
	friend XMVector operator*( float s, const Vector2& v );

	friend XMVector operator/( const Vector2& v, float s );

	friend Vector2& operator+=( Vector2& lhs, const Vector2& rhs );
	friend XMVector& operator+=( XMVector& lhs, const Vector2& rhs );
	friend Vector2& operator+=( Vector2& lhs, const XMVector& rhs );

	friend Vector2& operator-=( Vector2& lhs, const Vector2& rhs );
	friend XMVector& operator-=( XMVector& lhs, const Vector2& rhs );
	friend Vector2& operator-=( Vector2& lhs, const XMVector& rhs );

	friend Vector2& operator*=( Vector2& lhs, const Vector2& rhs );
	friend XMVector& operator*=( XMVector& lhs, const Vector2& rhs );
	friend Vector2& operator*=( Vector2& lhs, const XMVector& rhs );

	friend Vector2& operator/=( Vector2& lhs, const Vector2& rhs );
	friend XMVector& operator/=( XMVector& lhs, const Vector2& rhs );
	friend Vector2& operator/=( Vector2& lhs, const XMVector& rhs );

	friend Vector2& operator*=( Vector2& v, float s );
	friend Vector2& operator/=( Vector2& v, float s );

	friend XMVector operator+( const Vector2& v );
	friend XMVector operator-( const Vector2& v );

	operator XMVector() { return DirectX::XMLoadFloat2( this ); }
	operator const XMVector() const { return DirectX::XMLoadFloat2( this ); }

	static const Vector2 OneVector;
	static const Vector2 ZeroVector;
};

using Point2 = Vector2;
