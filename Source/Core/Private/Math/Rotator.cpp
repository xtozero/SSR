#include "Rotator.h"

#include "Quaternion.h"

using ::DirectX::XMConvertToRadians;

Quaternion Rotator::ToQuat() const
{
	return Quaternion( XMConvertToRadians( m_pitch ), XMConvertToRadians( m_yaw ), XMConvertToRadians( m_roll ) );
}

Rotator::Rotator( float pitch, float yaw, float roll )
	: m_pitch( pitch )
	, m_yaw( yaw )
	, m_roll( roll )
{
}

float& Rotator::operator[]( size_t idx )
{
	switch ( idx )
	{
	case 0:
		return m_pitch;
		break;
	case 1:
		return m_yaw;
		break;
	case 2:
		return m_roll;
		break;
	}

	__debugbreak();
	return m_pitch;
}

float Rotator::operator[]( size_t idx ) const
{
	switch ( idx )
	{
	case 0:
		return m_pitch;
		break;
	case 1:
		return m_yaw;
		break;
	case 2:
		return m_roll;
		break;
	}

	__debugbreak();
	return m_pitch;
}

Rotator operator+( const Rotator& lhs, const Rotator& rhs )
{
	return Rotator( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2] );
}

Rotator operator-( const Rotator& lhs, const Rotator& rhs )
{
	return Rotator( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2] );
}

bool operator==( const Rotator& lhs, const Rotator& rhs )
{
	return lhs[0] == rhs[0] 
		&& lhs[1] == rhs[1]
		&& lhs[2] == rhs[2];
}

Rotator operator*( const Rotator& r, float s )
{
	return Rotator( r[0] * s, r[1] * s, r[2] * s );
}

Rotator operator*( float s, const Rotator& r )
{
	return r * s;
}

Rotator operator/( const Rotator& r, float s )
{
	return Rotator( r[0] / s, r[1] / s, r[2] / s );
}

Rotator& operator+=( Rotator& lhs, const Rotator& rhs )
{
	lhs[0] += rhs[0];
	lhs[1] += rhs[1];
	lhs[2] += rhs[2];

	return lhs;
}

Rotator& operator-=( Rotator& lhs, const Rotator& rhs )
{
	lhs[0] -= rhs[0];
	lhs[1] -= rhs[1];
	lhs[2] -= rhs[2];

	return lhs;
}

Rotator& operator*=( Rotator& r, float s )
{
	r[0] *= s;
	r[1] *= s;
	r[2] *= s;

	return r;
}

Rotator& operator/=( Rotator& r, float s )
{
	r[0] /= s;
	r[1] /= s;
	r[2] /= s;

	return r;
}
