#pragma once

#include "Physics/Contacts.h"
#include "SizedTypes.h"

class BoundingSphere;
class CAaboundingbox;
class COrientedBoundingBox;
class RigidBody;

struct CollisionData
{
	Contact* m_contactArray = nullptr;
	Contact* m_contacts = nullptr;
	int32 m_contactsLeft = 0;
	int32 m_contactsCount = 0;
	float m_friction;
	float m_restitution;
	float m_tolerance;

	void AddContacts( int32 count )
	{
		m_contactsLeft -= count;
		m_contactsCount += count;

		m_contacts += count;
	}

	void Reset( Contact* contactArray, int32 maxContacts )
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

uint32 SphereAndSphere( const BoundingSphere& lhs, RigidBody* lhsBody, const BoundingSphere& rhs, RigidBody* rhsBody, CollisionData* data );
uint32 SphereAndHalfSpace( const BoundingSphere& sphere, RigidBody* sphereBody, const CXMFLOAT4& plane, CollisionData* data );
uint32 SphereAndTruePlane( const BoundingSphere& sphere, RigidBody* sphereBody, const CXMFLOAT4& plane, CollisionData* data );
uint32 BoxAndHalfSpace( const CAaboundingbox& box, RigidBody* boxBody, const CXMFLOAT4& plane, CollisionData* data );
uint32 BoxAndSphere( const CAaboundingbox& box, RigidBody* boxBody, const BoundingSphere& sphere, RigidBody* sphereBody, CollisionData* data );
uint32 BoxAndSphere( const COrientedBoundingBox& box, RigidBody* boxBody, const BoundingSphere& sphere, RigidBody* sphereBody, CollisionData* data );
uint32 BoxAndBox( const CAaboundingbox& lhs, RigidBody* lhsBody, const CAaboundingbox& rhs, RigidBody* rhsBody, CollisionData* data );
uint32 BoxAndBox( const COrientedBoundingBox& lhs, RigidBody* lhsBody, const COrientedBoundingBox& rhs, RigidBody* rhsBody, CollisionData* data );
uint32 BoxAndBox( const CAaboundingbox& lhs, RigidBody* lhsBody, const COrientedBoundingBox& rhs, RigidBody* rhsBody, CollisionData* data );

float RayAndBox( const CXMFLOAT3& rayOrigin, const CXMFLOAT3& rayDir, const CXMFLOAT3& max, const CXMFLOAT3& min );
float RayAndSphere( const CXMFLOAT3& rayOrigin, const CXMFLOAT3& rayDir, const CXMFLOAT3& origin, float radius );