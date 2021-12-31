#pragma once

#include "Math/Vector.h"

#include <utility>

class CGroundMovement
{
public:
	void Update( Vector& force, float elapsedTime )
	{
		float magnitude = force.Length();
		if ( magnitude > m_maxForceMag )
		{
			force *= m_maxForceMag / magnitude;
			magnitude = m_maxForceMag;
		}

		if ( m_isMoving == false )
		{
			if ( m_friction.first <= magnitude )
			{
				m_isMoving = true;
			}
			else
			{
				return;
			}
		}

		Vector moveDir = m_velocity.GetNormalized();
		if ( m_isMoving )
		{
			// calculate net force
			force -= ( moveDir * m_friction.second );
		}

		assert( m_mass != 0.f );
		Vector acceleration = force / m_mass;
		m_velocity += acceleration * elapsedTime;

		if ( ( moveDir | m_velocity ) < 0.f )
		{
			m_isMoving = false;
			m_velocity = Vector::ZeroVector;
		}
	}

	Vector GetDelta( float elapsedTime ) const
	{
		return m_velocity * elapsedTime;
	}

	void SetFriction( const std::pair<float, float>& friction )
	{
		m_friction = friction;
	}

	void SetMaxForceMagnitude( float magnitude ) { m_maxForceMag = magnitude; }

private:
	float m_mass = 1.f;
	Vector m_velocity;

	float m_maxForceMag = FLT_MAX;

	std::pair<float, float> m_friction; // static, kinetic

	bool m_isMoving = false;
};
