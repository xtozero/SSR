#include "ParticleWorld.h"

#include "Particle.h"

ParticleWorld::ParticleWorld( uint32 iteration ) : m_resolver( iteration )
{
	m_calculateIterations = ( iteration == 0 );
}

void ParticleWorld::BootUp( int32 maxContacts )
{
	m_maxContacts = maxContacts;
	m_contacts = std::make_unique<ParticleContact[]>( m_maxContacts );
}

void ParticleWorld::PreparePhysics()
{
	for ( Particle* p : m_particles )
	{
		p->ClearAccumulator();
	}
}

void ParticleWorld::RunPhysics( float duration )
{
	m_registry.UpdateForces( duration );

	Integrate( duration );

	int32 usedContacts = GenerateContacts();

	if ( usedContacts > 0 )
	{
		if ( m_calculateIterations )
		{
			m_resolver.SetIterations( usedContacts * 2 );
		}
		m_resolver.ResolveContacts( m_contacts.get(), usedContacts, duration );
	}
}

int32 ParticleWorld::GenerateContacts()
{
	int32 limit = m_maxContacts;
	ParticleContact* nextContact = m_contacts.get();

	for ( ParticleContactGenerator* g : m_contactGenerators )
	{
		uint32 used = g->AddContact( nextContact, limit );
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
