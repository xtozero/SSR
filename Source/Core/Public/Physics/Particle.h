#pragma once

#include "Math/CXMFloat.h"

class Particle
{
public:
	void Integrate( float duration );

	void SetPosition( const CXMFLOAT3& position );
	CXMFLOAT3 GetPosition( ) const;

	void SetVelocity( const CXMFLOAT3& velocity );
	CXMFLOAT3 GetVelocity( ) const;

	void SetAcceleration( const CXMFLOAT3 acceleration );
	CXMFLOAT3 GetAcceleration( ) const;

	void SetDamping( float damping );
	float GetDamping( ) const;
	
	void SetMass( float mass );
	float GetMass( ) const;

	void SetInverseMass( float inverseMass );
	float GetInverseMass( ) const;

	void AddForce( const CXMFLOAT3& force );
	void ClearAccumulator( );

	bool HasFiniteMass( ) const { return m_inverseMass > 0.f; }

protected:
	CXMFLOAT3 m_position;
	CXMFLOAT3 m_velocity;
	CXMFLOAT3 m_acceleration;
	CXMFLOAT3 m_forceAccum;

	float m_damping = 0.999f;

	float m_inverseMass = 0.f;
};