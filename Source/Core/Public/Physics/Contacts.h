#pragma once

#include "Math/Matrix3X3.h"
#include "Math/Vector.h"
#include "SizedTypes.h"

class RigidBody;

class ContactResolver;

class Contact
{
public:
	void SetContactPoint( const Point& contactPoint ) { m_contactPoint = contactPoint; }
	void SetContactNormal( const Vector& contactNormal ) { m_contactNormal = contactNormal; }
	void SetPenetration( float penetration ) { m_penetration = penetration; }
	void SetBodyData( RigidBody* one, RigidBody* two, float friction, float restitution );

	void CalculateContactBasis( );
	Vector CalculateLocalVelocity( uint32 bodyIndex, float duration );
	void CalculateDesiredDeltaVelocity( float duration );
	void CalculateInternals( float duration );
	Vector CalculateFrictionlessImpulse( const Matrix3X3* inverseInertiaTensor );
	Vector CalculateFrictionImpulse( const Matrix3X3* inverseInertiaTensor );

	void ApplyPositionChange( Vector linearChange[2], Vector angularChange[2], float penetration );
	void ApplyVelocityChange( Vector velocityChange[2], Vector rotationChange[2] );

	friend ContactResolver;
private:
	void SwapBodies( );
	void MatchAwakeState( );

	RigidBody* m_body[2] = { nullptr, nullptr };
	Point m_contactPoint;
	Vector m_contactNormal;
	float m_penetration = 0.f;
	float m_friction = 0.f;
	float m_restitution = 0.f;

	Matrix3X3 m_contactToWorld;
	Vector m_contactVelocity;
	float m_desiredDeltaVelocity = 0.f;
	Point m_relativeContactPosition[2];
};

class ContactResolver
{
public:
	void Initialize( uint32 iterations, float velocityEpsilon = 0.01f, float positionEpsilon = 0.01f );
	void ResolveContacts( Contact* contactArray, uint32 numContacts, float duration );

private:
	void PrepareContacts( Contact* contactArray, uint32 numContacts, float duration );
	void AdjustPositions( Contact* contactArray, uint32 numContacts, float duration );
	void AdjustVelocities( Contact* contactArray, uint32 numContacts, float duration );

	uint32 m_positionIterations = 0;
	uint32 m_positionIterationsUsed = 0;

	uint32 m_velocityIterations = 0;
	uint32 m_velocityIterationsUsed = 0;

	float m_velocityEpsilon = 0.01f;
	float m_positionEpsilon = 0.01f;
};