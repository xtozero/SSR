#pragma once

struct Quaternion;

struct Rotator
{
public:
	Quaternion ToQuat() const;

	Rotator( float pitch, float yaw, float roll );
	Rotator() = default;

	float& operator[]( size_t idx );
	float operator[]( size_t idx ) const;

	friend Rotator operator+( const Rotator& lhs, const Rotator& rhs );

	friend Rotator operator-( const Rotator& lhs, const Rotator& rhs );

	friend bool operator==( const Rotator& lhs, const Rotator& rhs );

	friend Rotator operator*( const Rotator& r, float s );
	friend Rotator operator*( float s, const Rotator& r );

	friend Rotator operator/( const Rotator& r, float s );

	friend Rotator& operator+=( Rotator& lhs, const Rotator& rhs );

	friend Rotator& operator-=( Rotator& lhs, const Rotator& rhs );

	friend Rotator& operator*=( Rotator& r, float s );
	friend Rotator& operator/=( Rotator& r, float s );

	float m_pitch = 0.f;
	float m_yaw = 0.f;
	float m_roll = 0.f;

	static const Rotator ZeroRotator;
};


