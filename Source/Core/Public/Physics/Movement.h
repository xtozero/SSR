#pragma once

#include "Math/CXMFloat.h"

#include <utility>

class CGroundMovement
{
public:
	void Update( CXMFLOAT3& force, float elapsedTime )
	{
		float magnitude = DirectX::XMVectorGetX( DirectX::XMVector3Length( force ) );
		if ( magnitude > m_maxForceMag )
		{
			force *= m_maxForceMag / magnitude;
			magnitude = m_maxForceMag;
		}

		if ( m_isMoving == false )
		{
			if ( m_friction.x <= magnitude )
			{
				m_isMoving = true;
			}
			else
			{
				return;
			}
		}

		CXMFLOAT3 moveDir = DirectX::XMVector3Normalize( m_velocity );
		if ( m_isMoving )
		{
			// calculate net force
			force -= ( moveDir * m_friction.y );
		}

		assert( m_mass != 0.f );
		CXMFLOAT3 acceleration = force / m_mass;
		m_velocity += acceleration * elapsedTime;

		if ( DirectX::XMVectorGetX( DirectX::XMVector3Dot( moveDir, m_velocity ) ) < 0.f )
		{
			m_isMoving = false;
			m_velocity = CXMFLOAT3( 0.f, 0.f, 0.f );
		}
	}

	CXMFLOAT3 GetDelta( float elapsedTime ) const
	{
		return m_velocity * elapsedTime;
	}

	void SetFriction( const CXMFLOAT2& friction )
	{
		m_friction = friction;
	}

	void SetMaxForceMagnitude( float magnitude ) { m_maxForceMag = magnitude; }

private:
	float m_mass = 1.f;
	CXMFLOAT3 m_velocity;

	float m_maxForceMag = FLT_MAX;

	CXMFLOAT2 m_friction; // static, kinetic

	bool m_isMoving = false;
};
