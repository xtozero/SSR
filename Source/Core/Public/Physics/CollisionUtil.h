#pragma once

#include "Math/Matrix3X3.h"
#include "SizedTypes.h"

class CRay;
class ICollider;
class RigidBody;

enum class CollisionResult : uint8;

struct CollisionData;

namespace COLLISION_UTIL
{
	float IntersectWithRay( const ICollider& collider, const CRay& ray );
	CollisionResult DetectCollisionObjectAndObject( const ICollider* lhsCollider, RigidBody* lhsBody, const ICollider* rhsCollider, RigidBody* rhsBody, CollisionData* data );
	Matrix3X3 CalcInertiaTensor( const ICollider* collider, float mass );
}