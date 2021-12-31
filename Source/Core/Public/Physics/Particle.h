#pragma once

#include "Math/Vector.h"

class Particle
{
public:
	void Integrate( float duration );

	void SetPosition( const Point& position );
	Point GetPosition( ) const;

	void SetVelocity( const Vector& velocity );
	Vector GetVelocity( ) const;

	void SetAcceleration( const Vector acceleration );
	Vector GetAcceleration( ) const;

	void SetDamping( float damping );
	float GetDamping( ) const;
	
	void SetMass( float mass );
	float GetMass( ) const;

	void SetInverseMass( float inverseMass );
	float GetInverseMass( ) const;

	void AddForce( const Vector& force );
	void ClearAccumulator( );

	bool HasFiniteMass( ) const { return m_inverseMass > 0.f; }

protected:
	Point m_position;
	Vector m_velocity;
	Vector m_acceleration;
	Vector m_forceAccum;

	float m_damping = 0.999f;

	float m_inverseMass = 0.f;
};