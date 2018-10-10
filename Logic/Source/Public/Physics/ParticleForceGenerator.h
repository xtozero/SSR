#pragma once

#include "Math/CXMFloat.h"

#include <vector>

class Particle;

class ParticleForceGenerator
{
public:
	virtual void UpdateForce( Particle* particle, float duration ) = 0;
};

class ParticleGravity : public ParticleForceGenerator
{
public:
	explicit ParticleGravity( const CXMFLOAT3& gravity ) : m_gravity( gravity ) {}
	virtual void UpdateForce( Particle* particle, float duration ) override;

private:
	CXMFLOAT3 m_gravity;
};

class ParticleDrag : public ParticleForceGenerator
{
public:
	ParticleDrag( float k1, float k2 ) : m_k1( k1 ), m_k2( k2 ) {}
	virtual void UpdateForce( Particle* particle, float duration ) override;

private:
	float m_k1;
	float m_k2;
};

class ParticleSpring : public ParticleForceGenerator
{
public:
	ParticleSpring( Particle* other, float springConstant, float restLength ) : m_other( other ), m_springConstant( springConstant ), m_restLength( restLength ) {}
	virtual void UpdateForce( Particle* particle, float duration ) override;
	
private:
	Particle* m_other;
	float m_springConstant;
	float m_restLength;
};

class ParticleAnchoredSpring : public ParticleForceGenerator
{
public:
	ParticleAnchoredSpring( CXMFLOAT3* anchor, float springConstant, float restLength ) : m_anchor( anchor ), m_springConstant( springConstant ), m_restLength( restLength ) {}
	virtual void UpdateForce( Particle* particle, float duration ) override;

private:
	CXMFLOAT3* m_anchor;
	float m_springConstant;
	float m_restLength;
};

class ParticleBungee : public ParticleForceGenerator
{
public:
	ParticleBungee( Particle* other, float springConstant, float restLength ) : m_other( other ), m_springConstant( springConstant ), m_restLength( restLength ) {}
	virtual void UpdateForce( Particle* particle, float duration ) override;

private:
	Particle* m_other;
	float m_springConstant;
	float m_restLength;
};

class ParticleBuoyancy : public ParticleForceGenerator
{
public:
	ParticleBuoyancy( float maxDepth, float volume, float waterHeight, float liquidDensity = 1000.f ) : m_maxDepth( maxDepth ), m_volume( volume ), m_waterHeight( waterHeight ), m_liquidDensity( liquidDensity ) {}
	virtual void UpdateForce( Particle* particle, float duration ) override;

private:
	float m_maxDepth;
	float m_volume;
	float m_waterHeight;
	float m_liquidDensity = 1000.f;
};

class ParticleFakeSpring : public ParticleForceGenerator
{
public:
	ParticleFakeSpring( CXMFLOAT3* anchor, float springConstant, float damping ) : m_anchor( anchor ), m_springConstant( springConstant ), m_damping( damping ) {}
	virtual void UpdateForce( Particle* particle, float duration ) override;

private:
	CXMFLOAT3* m_anchor;
	float m_springConstant;
	float m_damping;
};

class ParticleForceRegistry
{
public:
	void Add( Particle* particle, ParticleForceGenerator* fg );
	void Remove( Particle* particle, ParticleForceGenerator* fg );

	void Clear( );

	void UpdateForces( float duration );

private:
	struct ParticleForceRegistration
	{
		ParticleForceRegistration( Particle* particle, ParticleForceGenerator* fg ) : m_particle( particle ), m_fg( fg ) {}

		Particle* m_particle;
		ParticleForceGenerator* m_fg;
	};

	using Registry = std::vector<ParticleForceRegistration>;
	Registry m_registrations;
};