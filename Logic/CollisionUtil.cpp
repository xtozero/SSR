#include "stdafx.h"
#include "CollisionUtil.h"

#include "GameObject.h"
#include "IRigidBody.h"

namespace COLLISION_UTIL
{
	float IntersectWithRay( const IRigidBody& rigidBody, const CRay& ray )
	{
		return rigidBody.Intersect( &ray );
	}

	float IntersectWithRay( const CGameObject& object, const CRay& ray )
	{
		for ( int i = 0; i < RIGID_BODY_TYPE::Count; ++i )
		{
			const IRigidBody* pRigidBody = object.GetRigidBody( i );
			if ( pRigidBody )
			{
				return IntersectWithRay( *pRigidBody, ray );
			}
		}

		return -1.f;
	}
}