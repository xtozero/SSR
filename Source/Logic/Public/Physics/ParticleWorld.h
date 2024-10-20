#pragma once
#include "Math/CXMFloat.h"
#include "ParticleContacts.h"
#include "ParticleForceGenerator.h"

#include <memory>
#include <vector>

class Particle;

class ParticleWorld
{
public:
	explicit ParticleWorld( int iteration = 0 );
	
	void BootUp( int maxContacts );

	void StartFrame( );
	void RunPhysics( float duration );

private:
	int GenerateContacts( );
	void Integrate( float duration );

	using Particles = std::vector<Particle*>;
	Particles m_particles;

	ParticleForceRegistry m_registry;

	ParticleContantResolver m_resolver;

	using ContactGenerators = std::vector<ParticleContactGenerator*>;
	ContactGenerators m_contactGenerators;

	std::unique_ptr<ParticleContact[]> m_contacts;
	int m_maxContacts = 0;
	bool m_calculateIterations = false;
};