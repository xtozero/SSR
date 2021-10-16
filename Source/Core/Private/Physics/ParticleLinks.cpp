#include "ParticleLinks.h"

#include "Particle.h"

using namespace DirectX;

float ParticleLink::CurrentLength( ) const
{
	CXMFLOAT3 relativePos = m_particle[0]->GetPosition( ) - m_particle[1]->GetPosition( );
	return XMVectorGetX( XMVector3Length( relativePos ) );
}

uint32 ParticleCable::AddContact( ParticleContact* contact, uint32 /*limit*/ ) const
{
	float length = CurrentLength( );
	if ( length < m_maxLength )
	{
		return 0;
	}

	contact->m_particle[0] = m_particle[0];
	contact->m_particle[1] = m_particle[1];

	CXMFLOAT3 normal = m_particle[1]->GetPosition( ) - m_particle[0]->GetPosition( );
	contact->m_contactNormal = XMVector3Normalize( normal );
	contact->m_penetration = length - m_maxLength;
	contact->m_restitution = m_restitution;

	return 1;
}

uint32 ParticleRod::AddContact( ParticleContact* contact, uint32 /*limit*/ ) const
{
	float currentlength = CurrentLength( );
	if ( currentlength == m_length )
	{
		return 0;
	}

	contact->m_particle[0] = m_particle[0];
	contact->m_particle[1] = m_particle[1];

	CXMFLOAT3 normal = m_particle[1]->GetPosition( ) - m_particle[0]->GetPosition( );
	
	
	if ( currentlength > m_length )
	{
		contact->m_contactNormal = XMVector3Normalize( normal );
		contact->m_penetration = currentlength - m_length;
	}
	else
	{
		contact->m_contactNormal = -XMVector3Normalize( normal );
		contact->m_penetration = m_length - currentlength;
	}

	contact->m_restitution = 0;

	return 1;
}
