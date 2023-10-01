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

enum class CollisionResult : uint8;

struct CollisionData
{
	Contact* m_contactArray = nullptr;
	Contact* m_contacts = nullptr;
	int32 m_contactsLeft = 0;
	int32 m_contactsCount = 0;
	float m_friction = 0.f;
	float m_restitution = 0.f;
	float m_tolerance = 0.f;

	void AddContacts( int32 count );

	void Reset( Contact* contactArray, int32 maxContacts );

	bool HasMoreContact( ) const;
};

CollisionResult SphereAndSphere( const BoundingSphere& lhs, RigidBody* lhsBody, const BoundingSphere& rhs, RigidBody* rhsBody, CollisionData* data );
CollisionResult SphereAndHalfSpace( const BoundingSphere& sphere, RigidBody* sphereBody, const Plane& plane, CollisionData* data );
CollisionResult SphereAndTruePlane( const BoundingSphere& sphere, RigidBody* sphereBody, const Plane& plane, CollisionData* data );
CollisionResult SphereAndFrusturm( const Point& origin, float radius, const Frustum& frustum );
CollisionResult SphereAndFrusturm( const Point& origin, float radius, const Frustum& frustum, const Vector& sweepDir );
CollisionResult BoxAndHalfSpace( const AxisAlignedBox& box, RigidBody* boxBody, const Plane& plane, CollisionData* data );
CollisionResult BoxAndSphere( const AxisAlignedBox& box, RigidBody* boxBody, const BoundingSphere& sphere, RigidBody* sphereBody, CollisionData* data );
CollisionResult BoxAndSphere( const OrientedBox& box, RigidBody* boxBody, const BoundingSphere& sphere, RigidBody* sphereBody, CollisionData* data );
CollisionResult BoxAndBox( const AxisAlignedBox& lhs, RigidBody* lhsBody, const AxisAlignedBox& rhs, RigidBody* rhsBody, CollisionData* data );
CollisionResult BoxAndBox( const OrientedBox& lhs, RigidBody* lhsBody, const OrientedBox& rhs, RigidBody* rhsBody, CollisionData* data );
CollisionResult BoxAndBox( const AxisAlignedBox& lhs, RigidBody* lhsBody, const OrientedBox& rhs, RigidBody* rhsBody, CollisionData* data );
CollisionResult BoxAndFrustum( const Point& min, const Point& max, const Frustum& frustum );

float RayAndBox( const Point& rayOrigin, const Vector& rayDir, const Point& max, const Point& min );
float RayAndSphere( const Point& rayOrigin, const Vector& rayDir, const Point& origin, float radius );