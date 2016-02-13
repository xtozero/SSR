#pragma once

class CRay;
class IRigidBody;
class CGameObject;

namespace COLLISION_UTIL
{
	float IntersectWithRay( const CGameObject& object, const CRay& ray );
}