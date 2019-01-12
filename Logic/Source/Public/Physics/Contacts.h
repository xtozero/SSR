#pragma once

#include "Math\CXMFloat.h"

class RigidBody;

class ContactResolver;

class Contact
{
public:
	void SetContactPoint( const CXMFLOAT3& contactPoint ) { m_contactPoint = contactPoint; }
	void SetContactNormal( const CXMFLOAT3& contactNormal ) { m_contactNormal = contactNormal; }
	void SetPenetration( float penetration ) { m_penetration = penetration; }
	void SetBodyData( RigidBody* one, RigidBody* two, float friction, float restitution );

	void CalculateContactBasis( );
	CXMFLOAT3 CalculateLocalVelocity( int bodyIndex, float duration );
	void CalculateDesiredDeltaVelocity( float duration );
	void CalculateInternals( float duration );
	CXMFLOAT3 CalculateFrictionlessImpulse( const CXMFLOAT3X3* inverseInertiaTensor );
	CXMFLOAT3 CalculateFrictionImpulse( const CXMFLOAT3X3* inverseInertiaTensor );

	void ApplyPositionChange( CXMFLOAT3 linearChange[2], CXMFLOAT3 angularChange[2], float penetration );
	void ApplyVelocityChange( CXMFLOAT3 velocityChange[2], CXMFLOAT3 rotationChange[2] );

	friend ContactResolver;
private:
	void SwapBodies( );

	RigidBody* m_body[2] = { nullptr, nullptr };
	CXMFLOAT3 m_contactPoint;
	CXMFLOAT3 m_contactNormal;
	float m_penetration;
	float m_friction;
	float m_restitution;

	CXMFLOAT3X3 m_contactToWorld;
	CXMFLOAT3 m_contactVelocity;
	float m_desiredDeltaVelocity;
	CXMFLOAT3 m_relativeContactPosition[2];
};

class ContactResolver
{
public:
	void Initialize( int iterations, float velocityEpsilon = 0.01f, float positionEpsilon = 0.01f );
	void ResolveContacts( Contact* contactArray, int numContacts, float duration );

private:
	void PrepareContacts( Contact* contactArray, int numContacts, float duration );
	void AdjustPositions( Contact* contactArray, int numContacts, float duration );
	void AdjustVelocities( Contact* contactArray, int numContacts, float duration );

	int m_positionIterations = 0;
	int m_positionIterationsUsed = 0;

	int m_velocityIterations = 0;
	int m_velocityIterationsUsed = 0;

	float m_velocityEpsilon = 0.01f;
	float m_positionEpsilon = 0.01f;
};