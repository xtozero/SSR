#include "Particle.h"

void Particle::Integrate( float duration )
{
	if ( m_inverseMass <= 0.f )
	{
		return;
	}

	assert( duration > 0.f );

	m_position += m_velocity * duration;

	Vector resultingAcc = m_acceleration;
	resultingAcc += m_forceAccum * m_inverseMass;

	m_velocity += resultingAcc * duration;

	m_velocity *= powf( m_damping, duration );

	ClearAccumulator( );
}

void Particle::SetPosition( const Point& position )
{
	m_position = position;
}

Point Particle::GetPosition( ) const
{
	return m_position;
}

void Particle::SetVelocity( const Vector& velocity )
{
	m_velocity = velocity;
}

Vector Particle::GetVelocity( ) const
{
	return m_velocity;
}

void Particle::SetAcceleration( const Vector acceleration )
{
	m_acceleration = acceleration;
}

Vector Particle::GetAcceleration( ) const
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

void Particle::AddForce( const Vector& force )
{
	m_forceAccum += force;
}

void Particle::ClearAccumulator( )
{
	m_forceAccum = Vector::ZeroVector;
}
