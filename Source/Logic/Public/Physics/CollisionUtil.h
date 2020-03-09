#pragma once

class CRay;
class CGameObject;
class ICollider;
class RigidBody;
struct CollisionData;

namespace COLLISION_UTIL
{
	float IntersectWithRay( const ICollider& collider, const CRay& ray );
	unsigned int DetectCollisionObjectAndObject( CGameObject* lhs, RigidBody* lhsBody, CGameObject* rhs, RigidBody* rhsBody, CollisionData* data );
}