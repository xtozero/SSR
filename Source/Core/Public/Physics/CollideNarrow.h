#pragma once

#include "Math/Plane.h"
#include "Math/Vector.h"
#include "SizedTypes.h"

class BoundingSphere;
class AxisAlignedBox;
class Contact;
class OrientedBox;
class Frustum;
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

	void AddContacts( int32 count );

	void Reset( Contact* contactArray, int32 maxContacts );

	bool HasMoreContact( ) const;
};

uint32 SphereAndSphere( const BoundingSphere& lhs, RigidBody* lhsBody, const BoundingSphere& rhs, RigidBody* rhsBody, CollisionData* data );
uint32 SphereAndHalfSpace( const BoundingSphere& sphere, RigidBody* sphereBody, const Plane& plane, CollisionData* data );
uint32 SphereAndTruePlane( const BoundingSphere& sphere, RigidBody* sphereBody, const Plane& plane, CollisionData* data );
uint32 SphereAndFrusturm( const Point& origin, float radius, const Frustum& frustum );
bool SphereAndFrusturm( const Point& origin, float radius, const Frustum& frustum, const Vector& sweepDir );
uint32 BoxAndHalfSpace( const AxisAlignedBox& box, RigidBody* boxBody, const Plane& plane, CollisionData* data );
uint32 BoxAndSphere( const AxisAlignedBox& box, RigidBody* boxBody, const BoundingSphere& sphere, RigidBody* sphereBody, CollisionData* data );
uint32 BoxAndSphere( const OrientedBox& box, RigidBody* boxBody, const BoundingSphere& sphere, RigidBody* sphereBody, CollisionData* data );
uint32 BoxAndBox( const AxisAlignedBox& lhs, RigidBody* lhsBody, const AxisAlignedBox& rhs, RigidBody* rhsBody, CollisionData* data );
uint32 BoxAndBox( const OrientedBox& lhs, RigidBody* lhsBody, const OrientedBox& rhs, RigidBody* rhsBody, CollisionData* data );
uint32 BoxAndBox( const AxisAlignedBox& lhs, RigidBody* lhsBody, const OrientedBox& rhs, RigidBody* rhsBody, CollisionData* data );
uint32 BoxAndFrustum( const Point& min, const Point& max, const Frustum& frustum );

float RayAndBox( const Point& rayOrigin, const Vector& rayDir, const Point& max, const Point& min );
float RayAndSphere( const Point& rayOrigin, const Vector& rayDir, const Point& origin, float radius );