#include "ParticleContacts.h"

#include "Particle.h"

using namespace DirectX;

void ParticleContact::Resolve( float duration )
{
	ResolveVelocity( duration );
	ResolveInterpenetration( duration );
}

float ParticleContact::CalculateSeparatingVelocity( ) const
{
	Vector relativeVelocity = m_particle[0]->GetVelocity( );
	if ( m_particle[1] )
	{
		relativeVelocity -= m_particle[1]->GetVelocity( );
	}
	return relativeVelocity | m_contactNormal;
}

void ParticleContact::ResolveVelocity( float duration )
{
	float separatingVelocity = CalculateSeparatingVelocity( );
	
	if ( separatingVelocity > 0.f )
	{
		return;
	}

	float newSepVelocity = -separatingVelocity * m_restitution;
	
	Vector accCausedVelocity = m_particle[0]->GetAcceleration( );
	if ( m_particle[1] )
	{
		accCausedVelocity -= m_particle[1]->GetAcceleration( );
	}

	float accCausedSepVelocity = ( accCausedVelocity | m_contactNormal ) * duration;
	if ( accCausedSepVelocity < 0 )
	{
		newSepVelocity += m_restitution * accCausedSepVelocity;

		if ( newSepVelocity < 0 )
		{
			newSepVelocity = 0;
		}
	}
	
	float deltaVelocity = newSepVelocity - separatingVelocity;

	float totalInverseMass = m_particle[0]->GetInverseMass( );
	if ( m_particle[1] )
	{
		totalInverseMass += m_particle[1]->GetInverseMass( );
	}

	if ( totalInverseMass <= 0 )
	{
		return;
	}

	float impuse = deltaVelocity / totalInverseMass;
	Vector impusePerIMass = m_contactNormal * impuse;

	m_particle[0]->SetVelocity( m_particle[0]->GetVelocity( ) + impusePerIMass * m_particle[0]->GetInverseMass( ) );
	if ( m_particle[1] )
	{
		m_particle[1]->SetVelocity( m_particle[1]->GetVelocity( ) + impusePerIMass * -m_particle[1]->GetInverseMass( ) );
	}
}

void ParticleContact::ResolveInterpenetration( float /*duration*/ )
{
	if ( m_penetration <= 0 )
	{
		return;
	}

	float totalInverseMass = m_particle[0]->GetInverseMass( );
	if ( m_particle[1] )
	{
		totalInverseMass += m_particle[1]->GetInverseMass( );
	}

	if ( totalInverseMass <= 0 )
	{
		return;
	}

	Vector movePerIMass = m_contactNormal * ( m_penetration / totalInverseMass );

	m_particleMovement[0] = movePerIMass * m_particle[0]->GetInverseMass( );
	if ( m_particle[1] )
	{
		m_particleMovement[1] = movePerIMass * m_particle[1]->GetInverseMass( );
	}
	else
	{
		m_particleMovement[1] = Vector::ZeroVector;
	}

	m_particle[0]->SetPosition( m_particle[0]->GetPosition( ) + m_particleMovement[0] );
	if ( m_particle[1] )
	{
		m_particle[1]->SetPosition( m_particle[1]->GetPosition( ) + m_particleMovement[1] );
	}
}

void ParticleContactResolver::ResolveContacts( ParticleContact* contactArray, uint32 numContacts, float duration )
{
	for ( m_iterationUsed = 0; m_iterationUsed < m_iterations; ++m_iterations )
	{
		float max = FLT_MAX;
		uint32 maxIndex = numContacts;
		for ( uint32 i = 0; i < numContacts; ++i )
		{
			float sepVel = contactArray[i].CalculateSeparatingVelocity( );
			if ( sepVel < max && ( sepVel < 0 || contactArray[i].m_penetration > 0 ) )
			{
				max = sepVel;
				maxIndex = i;
			}
		}

		if ( maxIndex == numContacts )
		{
			break;
		}

		contactArray[maxIndex].Resolve( duration );
	}
}
