#include "stdafx.h"
#include "Physics/Particle.h"

void Particle::Integrate( float duration )
{
	if ( m_inverseMass <= 0.f )
	{
		return;
	}

	assert( duration > 0.f );

	m_position += m_velocity * duration;

	CXMFLOAT3 resultingAcc = m_acceleration;
	resultingAcc += m_forceAccum * m_inverseMass;

	m_velocity += resultingAcc * duration;

	m_velocity *= powf( m_damping, duration );

	ClearAccumulator( );
}

void Particle::SetPosition( const CXMFLOAT3& position )
{
	m_position = position;
}

CXMFLOAT3 Particle::GetPosition( ) const
{
	return m_position;
}

void Particle::SetVelocity( const CXMFLOAT3& velocity )
{
	m_velocity = velocity;
}

CXMFLOAT3 Particle::GetVelocity( ) const
{
	return m_velocity;
}

void Particle::SetAcceleration( const CXMFLOAT3 acceleration )
{
	m_acceleration = acceleration;
}

CXMFLOAT3 Particle::GetAcceleration( ) const
{
	return m_acceleration;
}

void Particle::SetDamping( float damping )
{
	m_damping = damping;
}

float Particle::GetDamping( ) const
{
	return m_damping;
}

void Particle::SetMass( float mass )
{
	assert( mass > 0.f );
	m_inverseMass = 1.f / mass;
}

float Particle::GetMass( ) const
{
	if ( m_inverseMass == 0.f )
	{
		return FLT_MAX;
	}
	else
	{
		return 1.f / m_inverseMass;
	}
}

void Particle::SetInverseMass( float inverseMass )
{
	m_inverseMass = inverseMass;
}

float Particle::GetInverseMass( ) const
{
	return m_inverseMass;
}

void Particle::AddForce( const CXMFLOAT3& force )
{
	m_forceAccum += force;
}

void Particle::ClearAccumulator( )
{
	m_forceAccum = { 0.f, 0.f, 0.f };
}
