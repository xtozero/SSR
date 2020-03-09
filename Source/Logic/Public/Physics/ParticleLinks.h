#pragma once
#include "ParticleContacts.h"

#include "Math/CXMFloat.h"

class ParticleLink : public ParticleContactGenerator
{
public:
	virtual int AddContant( ParticleContact* contact, int limit ) const = 0;

	Particle* m_particle[2];

protected:
	float CurrentLength( ) const;
};

class ParticleCable : public ParticleLink
{
public:
	virtual int AddContant( ParticleContact* contact, int limit ) const override;

	float m_maxLength;
	float m_restitution;
};

class ParticleRod : public ParticleLink
{
public:
	virtual int AddContant( ParticleContact* contact, int limit ) const override;

	float m_length;
};