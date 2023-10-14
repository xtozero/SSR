#pragma once

#include "Math/Matrix.h"
#include "Math/XMVector.h"

struct Rotator;

struct Quaternion : public DirectX::XMFLOAT4
{
public:
	float Length() const;
	XMVector GetNormalized() const;
	XMVector Inverse() const;
	bool Equals( const Quaternion& other, float tolerance = 1.e-4 ) const;

	Rotator ToRotator() const;

	Quaternion( float pitch, float yaw, float roll );
	Quaternion( const XMVector& v );
	explicit Quaternion( const Matrix& m );
	Quaternion() : DirectX::XMFLOAT4() {}
	using ::DirectX::XMFLOAT4::XMFLOAT4;

	Quaternion& operator=( const XMVector& v );

	float& operator[]( size_t idx );
	float operator[]( size_t idx ) const;

	friend XMVector operator+( const Quaternion& lhs, const Quaternion& rhs );
	friend XMVector operator+( const XMVector& lhs, const Quaternion& rhs );
	friend XMVector operator+( const Quaternion& lhs, const XMVector& rhs );

	friend XMVector operator-( const Quaternion& lhs, const Quaternion& rhs );
	friend XMVector operator-( const XMVector& lhs, const Quaternion& rhs );
	friend XMVector operator-( const Quaternion& lhs, const XMVector& rhs );

	friend XMVector operator*( const Quaternion& lhs, const Quaternion& rhs );
	friend XMVector operator*( const XMVector& lhs, const Quaternion& rhs );
	friend XMVector operator*( const Quaternion& lhs, const XMVector& rhs );

	friend float operator|( const Quaternion& lhs, const Quaternion& rhs );
	friend float operator|( const XMVector& lhs, const Quaternion& rhs );
	friend float operator|( const Quaternion& lhs, const XMVector& rhs );

	friend bool operator==( const Quaternion& lhs, const Quaternion& rhs );
	friend bool operator==( const XMVector& lhs, const Quaternion& rhs );
	friend bool operator==( const Quaternion& lhs, const XMVector& rhs );

	friend XMVector operator*( const Quaternion& v, float s );
	friend XMVector operator*( float s, const Quaternion& v );

	friend XMVector operator/( const Quaternion& v, float s );

	friend Quaternion& operator+=( Quaternion& lhs, const Quaternion& rhs );
	friend XMVector& operator+=( XMVector& lhs, const Quaternion& rhs );
	friend Quaternion& operator+=( Quaternion& lhs, const XMVector& rhs );

	friend Quaternion& operator-=( Quaternion& lhs, const Quaternion& rhs );
	friend XMVector& operator-=( XMVector& lhs, const Quaternion& rhs );
	friend Quaternion& operator-=( Quaternion& lhs, const XMVector& rhs );

	friend Quaternion& operator*=( Quaternion& lhs, const Quaternion& rhs );
	friend XMVector& operator*=( XMVector& lhs, const Quaternion& rhs );
	friend Quaternion& operator*=( Quaternion& lhs, const XMVector& rhs );

	friend Quaternion& operator*=( Quaternion& v, float s );
	friend Quaternion& operator/=( Quaternion& v, float s );

	operator XMVector() { return DirectX::XMLoadFloat4( this ); }
	operator const XMVector() const { return DirectX::XMLoadFloat4( this ); }

	static const Quaternion Identity;
};
