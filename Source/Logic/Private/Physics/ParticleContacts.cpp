#include "stdafx.h"
#include "Physics/ParticleContacts.h"

#include "Physics/Particle.h"

using namespace DirectX;

void ParticleContact::Resolve( float duration )
{
	ResolveVelocity( duration );
	ResolveInterpenetration( duration );
}

float ParticleContact::CalculateSeparatingVelocity( ) const
{
	CXMFLOAT3 relativeVelocity = m_particle[0]->GetVelocity( );
	if ( m_particle[1] )
	{
		relativeVelocity -= m_particle[1]->GetVelocity( );
	}
	return XMVectorGetX( XMVector3Dot( relativeVelocity, m_contactNormal ) );
}

void ParticleContact::ResolveVelocity( float duration )
{
	float separatingVelocity = CalculateSeparatingVelocity( );
	
	if ( separatingVelocity > 0.f )
	{
		return;
	}

	float newSepVelocity = -separatingVelocity * m_restitution;
	
	CXMFLOAT3 accCausedVelocity = m_particle[0]->GetAcceleration( );
	if ( m_particle[1] )
	{
		accCausedVelocity -= m_particle[1]->GetAcceleration( );
	}

	float accCausedSepVelocity = XMVectorGetX( XMVector3Dot( accCausedVelocity, m_contactNormal ) ) * duration;
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
	CXMFLOAT3 impusePerIMass = m_contactNormal * impuse;

	m_particle[0]->SetVelocity( m_particle[0]->GetVelocity( ) + CXMFLOAT3( impusePerIMass * m_particle[0]->GetInverseMass( ) ) );
	if ( m_particle[1] )
	{
		m_particle[1]->SetVelocity( m_particle[1]->GetVelocity( ) + CXMFLOAT3( impusePerIMass * -m_particle[1]->GetInverseMass( ) ) );
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

	CXMFLOAT3 movePerIMass = m_contactNormal * ( m_penetration / totalInverseMass );

	m_particleMovement[0] = movePerIMass * m_particle[0]->GetInverseMass( );
	if ( m_particle[1] )
	{
		m_particleMovement[1] = movePerIMass * m_particle[1]->GetInverseMass( );
	}
	else
	{
		m_particleMovement[1] = { 0.f, 0.f, 0.f };
	}

	m_particle[0]->SetPosition( m_particle[0]->GetPosition( ) + m_particleMovement[0] );
	if ( m_particle[1] )
	{
		m_particle[1]->SetPosition( m_particle[1]->GetPosition( ) + m_particleMovement[1] );
	}
}

void ParticleContantResolver::ResolveContants( ParticleContact* contactArray, int numContacts, float duration )
{
	for ( m_iterationUsed = 0; m_iterationUsed < m_iterations; ++m_iterations )
	{
		float max = FLT_MAX;
		int maxIndex = numContacts;
		for ( int i = 0; i < numContacts; ++i )
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
