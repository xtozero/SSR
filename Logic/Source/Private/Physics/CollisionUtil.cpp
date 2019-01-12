#include "stdafx.h"
#include "Physics/CollisionUtil.h"

#include "GameObject/GameObject.h"
#include "Physics/CollideNarrow.h"
#include "Physics/ICollider.h"

namespace COLLISION_UTIL
{
	float IntersectWithRay( const ICollider& collider, const CRay& ray )
	{
		return collider.Intersect( &ray );
	}

	unsigned int DetectCollisionObjectAndObject( CGameObject* lhs, RigidBody* lhsBody, CGameObject* rhs, RigidBody* rhsBody, CollisionData* data )
	{
		assert( lhs && rhs && lhsBody && rhsBody && data );
		COLLIDER::TYPE lhsType = lhs->GetColliderType( );
		COLLIDER::TYPE rhsType = rhs->GetColliderType( );

		if ( lhsType > rhsType )
		{
			std::swap( lhsType, rhsType );
			std::swap( lhs, rhs );
			std::swap( lhsBody, rhsBody );
		}

		if ( lhsType == COLLIDER::SPHERE )
		{
			const BoundingSphere* lhsCollider = reinterpret_cast<const BoundingSphere*>( lhs->GetDefaultCollider( ) );

			if ( rhsType == COLLIDER::SPHERE )
			{
				const BoundingSphere* rhsCollider = reinterpret_cast<const BoundingSphere*>( rhs->GetDefaultCollider( ) );
				return SphereAndSphere( *lhsCollider, lhsBody, *rhsCollider, rhsBody, data );
			}
			else if ( rhsType == COLLIDER::AABB )
			{
				const CAaboundingbox* boxCollider = reinterpret_cast<const CAaboundingbox*>( rhs->GetDefaultCollider( ) );
				return BoxAndSphere( *boxCollider, rhsBody, *lhsCollider , lhsBody, data );
			}
		}
		else if ( lhsType == COLLIDER::AABB )
		{
			const CAaboundingbox* boxCollider1 = reinterpret_cast<const CAaboundingbox*>( lhs->GetDefaultCollider( ) );

			if ( rhsType == COLLIDER::AABB )
			{
				const CAaboundingbox* boxCollider2 = reinterpret_cast<const CAaboundingbox*>( rhs->GetDefaultCollider( ) );
				return BoxAndBox( *boxCollider1, lhsBody, *boxCollider2, rhsBody, data );
			}
		}

		return 0;
	}

	float IntersectWithRay( CGameObject& object, const CRay& ray, COLLIDER::TYPE type )
	{
		if ( type >= COLLIDER::COUNT || type < COLLIDER::SPHERE )
		{
			for ( int i = 0; i < COLLIDER::COUNT; ++i )
			{
				if ( const ICollider* pCollider = object.GetCollider( i ) )
				{
					return IntersectWithRay( *pCollider, ray );
				}
			}
		}
		else
		{
			if ( const ICollider* pCollider = object.GetCollider( type ) )
			{
				return IntersectWithRay( *pCollider, ray );
			}
		}

		return -1.f;
	}
}