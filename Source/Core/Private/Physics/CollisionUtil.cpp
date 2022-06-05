#include "CollisionUtil.h"

#include "AxisAlignedBox.h"
#include "BoundingSphere.h"
#include "CollideNarrow.h"
#include "ICollider.h"
#include "OrientedBox.h"

namespace
{
	Matrix3X3 MakeInertiaTensorCoeffs( float ix, float iy, float iz, float ixy = 0, float iyz = 0, float ixz = 0 )
	{
		return Matrix3X3(
			ix, -ixy, -ixz,
			-ixy, iy, -iyz,
			-ixz, -iyz, iz );
	}

	Matrix3X3 MakeBlockInertiaTensor( const Vector& halfSizes, float mass )
	{
		if ( mass == FLT_MAX )
		{
			return Matrix3X3( 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f );
		}

		Vector squares = halfSizes * halfSizes;
		return MakeInertiaTensorCoeffs( 0.3f * mass * ( squares.y + squares.z ),
			0.3f * mass * ( squares.x + squares.z ),
			0.3f * mass * ( squares.x + squares.y ) );
	}

	Matrix3X3 MakeSphereInertiaTensor( const float radius, float mass )
	{
		if ( mass == FLT_MAX )
		{
			return Matrix3X3( 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f );
		}

		float coeff = 0.4f * mass * radius * radius;
		return MakeInertiaTensorCoeffs( coeff, coeff, coeff );
	}
}

namespace COLLISION_UTIL
{
	float IntersectWithRay( const ICollider& collider, const CRay& ray )
	{
		return collider.Intersect( ray );
	}

	uint32 DetectCollisionObjectAndObject( const ICollider* lhsCollider, RigidBody* lhsBody, const ICollider* rhsCollider, RigidBody* rhsBody, CollisionData* data )
	{
		Collider lhsType = lhsCollider->GetType();
		Collider rhsType = rhsCollider->GetType();

		if ( lhsType > rhsType )
		{
			std::swap( lhsType, rhsType );
			std::swap( lhsCollider, rhsCollider );
			std::swap( lhsBody, rhsBody );
		}

		if ( lhsType == Collider::Sphere )
		{
			const BoundingSphere* sphereCollider1 = reinterpret_cast<const BoundingSphere*>( lhsCollider );

			if ( rhsType == Collider::Sphere )
			{
				const BoundingSphere* sphereCollider2 = reinterpret_cast<const BoundingSphere*>( rhsCollider );
				return SphereAndSphere( *sphereCollider1, lhsBody, *sphereCollider2, rhsBody, data );
			}
			else if ( rhsType == Collider::Aabb )
			{
				const AxisAlignedBox* boxCollider = reinterpret_cast<const AxisAlignedBox*>( rhsCollider );
				return BoxAndSphere( *boxCollider, rhsBody, *sphereCollider1, lhsBody, data );
			}
			else if ( rhsType == Collider::Obb )
			{
				const OrientedBox* boxCollider = reinterpret_cast<const OrientedBox*>( rhsCollider );
				return BoxAndSphere( *boxCollider, rhsBody, *sphereCollider1, lhsBody, data );
			}
		}
		else if ( lhsType == Collider::Aabb )
		{
			const AxisAlignedBox* boxCollider1 = reinterpret_cast<const AxisAlignedBox*>( lhsCollider );

			if ( rhsType == Collider::Aabb )
			{
				const AxisAlignedBox* boxCollider2 = reinterpret_cast<const AxisAlignedBox*>( rhsCollider );
				return BoxAndBox( *boxCollider1, lhsBody, *boxCollider2, rhsBody, data );
			}
			else if ( rhsType == Collider::Obb )
			{
				const OrientedBox* boxCollider2 = reinterpret_cast<const OrientedBox*>( rhsCollider );
				return BoxAndBox( *boxCollider1, lhsBody, *boxCollider2, rhsBody, data );
			}
		}
		else if ( lhsType == Collider::Obb )
		{
			const OrientedBox* boxCollider1 = reinterpret_cast<const OrientedBox*>( lhsCollider );

			if ( rhsType == Collider::Obb )
			{
				const OrientedBox* boxCollider2 = reinterpret_cast<const OrientedBox*>( rhsCollider );
				return BoxAndBox( *boxCollider1, lhsBody, *boxCollider2, rhsBody, data );
			}
		}

		return 0;
	}

	Matrix3X3 CalcInertiaTensor( const ICollider* collider, float mass )
	{
		Collider type = collider->GetType();

		switch ( type )
		{
		case Collider::None:
			break;
		case Collider::Sphere:
		{
			auto sphere = static_cast<const BoundingSphere*>( collider );
			return MakeSphereInertiaTensor( sphere->GetRadius(), mass);
			break;
		}
		case Collider::Aabb:
		{
			auto aabb = static_cast<const AxisAlignedBox*>( collider );
			Vector halfSize = ( aabb->GetMax() - aabb->GetMin() ) * 0.5f;
			return MakeBlockInertiaTensor( halfSize, mass );
			break;
		}
		case Collider::Obb:
		{
			auto obb = static_cast<const OrientedBox*>( collider );
			return MakeBlockInertiaTensor( obb->GetHalfSize(), mass);
			break;
		}
		case Collider::Count:
			break;
		default:
			break;
		}

		return Matrix3X3( 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f );
	}
}