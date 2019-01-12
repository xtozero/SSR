#pragma once

#include "Physics/Contacts.h"

class BoundingSphere;
class CAaboundingbox;
class RigidBody;

struct CollisionData
{
	Contact* m_contactArray = nullptr;
	Contact* m_contacts = nullptr;
	int m_contactsLeft = 0;
	int m_contactsCount = 0;
	float m_friction;
	float m_restitution;
	float m_tolerance;

	void AddContacts( int count )
	{
		m_contactsLeft -= count;
		m_contactsCount += count;

		m_contacts += count;
	}

	void Reset( Contact* contactArray, int maxContacts )
	{
		m_contactsLeft = maxContacts;
		m_contactsCount = 0;
		m_contactArray = contactArray;
		m_contacts = m_contactArray;
	}

	bool HasMoreContact( ) const
	{
		return m_contactsLeft > 0;
	}
};

unsigned int SphereAndSphere( const BoundingSphere& lhs, RigidBody* lhsBody, const BoundingSphere& rhs, RigidBody* rhsBody, CollisionData* data );
unsigned int SphereAndHalfSpace( const BoundingSphere& sphere, RigidBody* sphereBody, const CXMFLOAT4& plane, CollisionData* data );
unsigned int SphereAndTruePlane( const BoundingSphere& sphere, RigidBody* sphereBody, const CXMFLOAT4& plane, CollisionData* data );
unsigned int BoxAndHalfSpace( const CAaboundingbox& box, RigidBody* boxBody, const CXMFLOAT4& plane, CollisionData* data );
unsigned int BoxAndSphere( const CAaboundingbox& box, RigidBody* boxBody, const BoundingSphere& sphere, RigidBody* sphereBody, CollisionData* data );
unsigned int BoxAndBox( const CAaboundingbox& lhs, RigidBody* lhsBody, const CAaboundingbox& rhs, RigidBody* rhsBody, CollisionData* data );