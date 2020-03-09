#include "stdafx.h"
#include "Physics/ParticleForceGenerator.h"

#include "Physics/Particle.h"

#include <algorithm>

using namespace DirectX;

void ParticleGravity::UpdateForce( Particle* particle, float /*duration*/ )
{
	if ( particle->HasFiniteMass() == false )
	{
		return;
	}
	
	particle->AddForce( m_gravity * particle->GetMass( ) );
}

void ParticleDrag::UpdateForce( Particle* particle, float /*duration*/ )
{
	CXMFLOAT3 force = particle->GetVelocity( );

	float dragCoeff = XMVectorGetX( XMVector3Length( force ) );
	dragCoeff = m_k1 * dragCoeff + m_k2 * dragCoeff * dragCoeff;

	force = XMVector3Normalize( force );
	force *= -dragCoeff;
	particle->AddForce( force );
}

void ParticleSpring::UpdateForce( Particle* particle, float /*duration*/ )
{
	CXMFLOAT3 force = particle->GetPosition( );
	force -= m_other->GetPosition( );

	float magnitude = XMVectorGetX( XMVector3Length( force ) );
	magnitude = fabsf( magnitude - m_restLength );
	magnitude *= m_springConstant;

	force = XMVector3Normalize( force );
	force *= -magnitude;
	particle->AddForce( force );
}

void ParticleAnchoredSpring::UpdateForce( Particle* particle, float /*duration*/ )
{
	CXMFLOAT3 force = particle->GetPosition( );
	force -= *m_anchor;

	float magnitude = XMVectorGetX( XMVector3Length( force ) );
	magnitude = ( m_restLength - magnitude ) * m_springConstant;

	force = XMVector3Normalize( force );
	force *= magnitude;
	particle->AddForce( force );
}

void ParticleBungee::UpdateForce( Particle* particle, float /*duration*/ )
{
	CXMFLOAT3 force = particle->GetPosition( );
	force -= m_other->GetPosition( );

	float magnitude = XMVectorGetX( XMVector3Length( force ) );
	if ( magnitude <= m_restLength )
	{
		return;
	}

	magnitude = ( m_restLength - magnitude ) * m_springConstant;

	force = XMVector3Normalize( force );
	force *= magnitude;
	particle->AddForce( force );
}

void ParticleBuoyancy::UpdateForce( Particle* particle, float /*duration*/ )
{
	float depth = particle->GetPosition( ).y;

	if ( depth >= m_waterHeight + m_maxDepth )
	{
		return;
	}

	CXMFLOAT3 force( 0.f, 0.f, 0.f );

	if ( depth <= m_waterHeight - m_maxDepth )
	{
		force.y = m_liquidDensity * m_volume;
		particle->AddForce( force );
		return;
	}

	force.y = m_liquidDensity * m_volume * ( depth - m_maxDepth - m_waterHeight ) / ( 2 * m_maxDepth );
	particle->AddForce( force );
}

void ParticleFakeSpring::UpdateForce( Particle* particle, float duration )
{
	if ( particle->HasFiniteMass( ) == false )
	{
		return;
	}

	CXMFLOAT3 position = particle->GetPosition( );
	position -= *m_anchor;

	float gamma = 0.5f * sqrtf( 4 * m_springConstant - m_damping * m_damping );
	if ( gamma == 0.f )
	{
		return;
	}

	CXMFLOAT3 c = position * ( m_damping / ( 2.f * gamma ) ) + particle->GetVelocity( ) * ( 1.f / gamma );
	CXMFLOAT3 target = position * cosf( gamma * duration ) + c * sinf( gamma * duration );
	target *= expf( -0.5f * duration * m_damping );

	CXMFLOAT3 accel = ( target - position ) * ( 1.f / duration * duration ) - particle->GetVelocity( ) * duration;
	particle->AddForce( accel * particle->GetMass( ) );
}

void ParticleForceRegistry::Add( Particle* particle, ParticleForceGenerator* fg )
{
	m_registrations.emplace_back( particle, fg );
}

void ParticleForceRegistry::Remove( Particle* particle, ParticleForceGenerator* fg )
{
	auto pred = [particle, fg]( const ParticleForceRegistration& value )
				{
					return value.m_particle == particle && value.m_fg == fg;
				};

	m_registrations.erase( std::remove_if( m_registrations.begin( ), m_registrations.end( ), pred ), m_registrations.end( ) );
}

void ParticleForceRegistry::Clear( )
{
	m_registrations.clear( );
}

void ParticleForceRegistry::UpdateForces( float duration )
{
	for ( auto i = m_registrations.begin( ); i != m_registrations.end( ); ++i )
	{
		i->m_fg->UpdateForce( i->m_particle, duration );
	}
}
