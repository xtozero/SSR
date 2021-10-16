#include "CollisionUtil.h"

#include "ColliderManager.h"
#include "CollideNarrow.h"
#include "ICollider.h"

namespace COLLISION_UTIL
{
	float IntersectWithRay( const ICollider& collider, const CRay& ray )
	{
		return collider.Intersect( ray );
	}

	uint32 DetectCollisionObjectAndObject( CGameObject* lhs, RigidBody* lhsBody, CGameObject* rhs, RigidBody* rhsBody, CollisionData* data )
	{
		/*assert( lhs && rhs && lhsBody && rhsBody && data );
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
			else if ( rhsType == COLLIDER::OBB )
			{
				const COrientedBoundingBox* boxCollider = reinterpret_cast<const COrientedBoundingBox*>( rhs->GetDefaultCollider( ) );
				return BoxAndSphere( *boxCollider, rhsBody, *lhsCollider, lhsBody, data );
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
			else if ( rhsType == COLLIDER::OBB )
			{
				const COrientedBoundingBox* boxCollider2 = reinterpret_cast<const COrientedBoundingBox*>( rhs->GetDefaultCollider( ) );
				return BoxAndBox( *boxCollider1, lhsBody, *boxCollider2, rhsBody, data );
			}
		}
		else if ( lhsType == COLLIDER::OBB )
		{
			const COrientedBoundingBox* boxCollider1 = reinterpret_cast<const COrientedBoundingBox*>( lhs->GetDefaultCollider( ) );

			if ( rhsType == COLLIDER::OBB )
			{
				const COrientedBoundingBox* boxCollider2 = reinterpret_cast<const COrientedBoundingBox*>( rhs->GetDefaultCollider( ) );
				return BoxAndBox( *boxCollider1, lhsBody, *boxCollider2, rhsBody, data );
			}
		}*/

		return 0;
	}

	float IntersectWithRay( CGameObject& object, const CRay& ray, COLLIDER::TYPE type )
	{
		/*if ( type >= COLLIDER::COUNT || type < COLLIDER::SPHERE )
		{
			for ( uint32 i = 0; i < COLLIDER::COUNT; ++i )
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
		}*/

		return -1.f;
	}
}