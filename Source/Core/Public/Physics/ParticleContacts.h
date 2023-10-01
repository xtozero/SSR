#pragma once

#include "Math/Vector.h"
#include "SizedTypes.h"

class Particle;

class ParticleContact
{
public:
	void Resolve( float duration );
	float CalculateSeparatingVelocity() const;

	Particle* m_particle[2] = {};

	float m_restitution = 0.f;

	Vector m_contactNormal;

	float m_penetration = 0.f;

	Vector m_particleMovement[2];

private:
	void ResolveVelocity( float duration );
	void ResolveInterpenetration( float duration );
};

class ParticleContactResolver
{
public:
	ParticleContactResolver( uint32 iterations ) : m_iterations( iterations ) {}
	void SetIterations( uint32 iterations ) { m_iterations = iterations; }
	void ResolveContacts( ParticleContact* contactArray, uint32 numContacts, float duration );

private:
	uint32 m_iterations;
	uint32 m_iterationUsed = 0;
};

class ParticleContactGenerator
{
public:
	virtual uint32 AddContact( ParticleContact* contact, uint32 limit ) const = 0;
};