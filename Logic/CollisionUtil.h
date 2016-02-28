#pragma once

class CRay;
class IRigidBody;
class CGameObject;

enum RIGID_BODY_TYPE;

namespace COLLISION_UTIL
{
	float IntersectWithRay( const CGameObject& object, const CRay& ray, RIGID_BODY_TYPE type );
}