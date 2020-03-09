#pragma once

#include "Math/CXMFloat.h"

class Particle;

class ParticleContact
{
public:
	void Resolve( float duration );
	float CalculateSeparatingVelocity( ) const;

	Particle* m_particle[2];

	float m_restitution;

	CXMFLOAT3 m_contactNormal;

	float m_penetration;

	CXMFLOAT3 m_particleMovement[2];

private:
	void ResolveVelocity( float duration );
	void ResolveInterpenetration( float duration );
};

class ParticleContantResolver
{
public:
	ParticleContantResolver( int iterations ) : m_iterations( iterations ) {}
	void SetIterations( int iterations ) { m_iterations = iterations; }
	void ResolveContants( ParticleContact* contactArray, int numContacts, float duration );

private:
	int m_iterations;
	int m_iterationUsed;
};

class ParticleContactGenerator
{
public:
	virtual int AddContant( ParticleContact* contact, int limit ) const = 0;
};