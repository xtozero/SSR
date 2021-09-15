#pragma once
#include "Math/CXMFloat.h"
#include "ParticleContacts.h"
#include "ParticleForceGenerator.h"
#include "SizedTypes.h"

#include <memory>
#include <vector>

class Particle;

class ParticleWorld
{
public:
	explicit ParticleWorld( uint32 iteration = 0 );
	
	void BootUp( int32 maxContacts );

	void PreparePhysics( );
	void RunPhysics( float duration );

private:
	int32 GenerateContacts( );
	void Integrate( float duration );

	using Particles = std::vector<Particle*>;
	Particles m_particles;

	ParticleForceRegistry m_registry;

	ParticleContactResolver m_resolver;

	using ContactGenerators = std::vector<ParticleContactGenerator*>;
	ContactGenerators m_contactGenerators;

	std::unique_ptr<ParticleContact[]> m_contacts;
	int32 m_maxContacts = 0;
	bool m_calculateIterations = false;
};