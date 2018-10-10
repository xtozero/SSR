#include "stdafx.h"
#include "Physics/ParticleWorld.h"

#include "Physics/Particle.h"

using namespace DirectX;

ParticleWorld::ParticleWorld( int iteration ) : m_resolver( iteration )
{
	m_calculateIterations = ( iteration == 0 );
}

void ParticleWorld::BootUp( int maxContacts )
{
	m_maxContacts = maxContacts;
	m_contacts = std::make_unique<ParticleContact[]>( m_maxContacts );
}

void ParticleWorld::StartFrame( )
{
	for ( Particle* p : m_particles )
	{
		p->ClearAccumulator( );
	}
}

void ParticleWorld::RunPhysics( float duration )
{
	m_registry.UpdateForces( duration );

	Integrate( duration );

	int usedContacts = GenerateContacts( );

	if ( usedContacts )
	{
		if ( m_calculateIterations )
		{
			m_resolver.SetIterations( usedContacts * 2 );
		}
		m_resolver.ResolveContants( m_contacts.get(), usedContacts, duration );
	}
}

int ParticleWorld::GenerateContacts( )
{
	int limit = m_maxContacts;
	ParticleContact* nextContact = m_contacts.get();

	for ( ParticleContactGenerator* g : m_contactGenerators )
	{
		int used = g->AddContant( nextContact, limit );
		limit -= used;
		nextContact += used;

		if ( limit <= 0 )
		{
			break;
		}
	}

	return m_maxContacts - limit;
}

void ParticleWorld::Integrate( float duration )
{
	for ( Particle* p : m_particles )
	{
		p->Integrate( duration );
	}
}
