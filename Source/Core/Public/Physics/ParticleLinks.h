#pragma once

#include "ParticleContacts.h"
#include "SizedTypes.h"

class ParticleLink : public ParticleContactGenerator
{
public:
	virtual uint32 AddContact( ParticleContact* contact, uint32 limit ) const = 0;

	Particle* m_particle[2];

protected:
	float CurrentLength( ) const;
};

class ParticleCable : public ParticleLink
{
public:
	virtual uint32 AddContact( ParticleContact* contact, uint32 limit ) const override;

	float m_maxLength;
	float m_restitution;
};

class ParticleRod : public ParticleLink
{
public:
	virtual uint32 AddContact( ParticleContact* contact, uint32 limit ) const override;

	float m_length;
};