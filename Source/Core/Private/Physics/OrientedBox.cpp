#include "OrientedBox.h"

#include "AxisAlignedBox.h"
#include "CollideNarrow.h"
#include "Frustum.h"
#include "Math/TransformationMatrix.h"
#include "Ray.h"

namespace
{
	float TransformToAxis( const OrientedBox& box, const Vector& axis )
	{
		const Vector& halfSize = box.GetHalfSize();

		return halfSize.x * fabsf( axis | box.GetAxisVector( 0 ) ) +
			halfSize.y * fabsf( axis | box.GetAxisVector( 1 ) ) +
			halfSize.z * fabsf( axis | box.GetAxisVector( 2 ) );
	}

	float CalcPenetrationOnAxis( const OrientedBox& lhs, const OrientedBox& rhs, const Vector& axis, const Vector& toCentre )
	{
		float lhsProject = TransformToAxis( lhs, axis );
		float rhsProject = TransformToAxis( rhs, axis );

		float distance = fabsf( toCentre | axis );

		return lhsProject + rhsProject - distance;
	}
}

void OrientedBox::Update( const Vector& scaling, const Quaternion& rotation, const Vector& translation, ICollider* original )
{
	OrientedBox* orig = dynamic_cast<OrientedBox*>( original );
	if ( orig == nullptr )
	{
		return;
	}

	m_matTransform = ScaleRotationTranslationMatrix( Vector::OneVector, rotation, translation );
	m_matInvTransform = m_matTransform.Inverse();

	for ( uint32 i = 0; i < 3; ++i )
	{
		m_halfSize[i] = orig->m_halfSize[i] * scaling[i];
	}
}

float OrientedBox::Intersect( const CRay& ray ) const
{
	Vector rayOrigin = m_matInvTransform.TransformPosition( ray.GetOrigin() );
	Vector rayDir = m_matInvTransform.TransformVector( ray.GetDir() );

	return RayAndBox( rayOrigin, rayDir, m_halfSize, -m_halfSize );
}

CollisionResult OrientedBox::Intersect( const Frustum& /*frustum*/ ) const
{
	assert( false && "Not Implemented" );
	return CollisionResult::Outside;
}

BoxSphereBounds OrientedBox::Bounds() const
{
	Vector points[] = {
		Vector( m_halfSize.x, m_halfSize.y, m_halfSize.z ),
		Vector( m_halfSize.x, m_halfSize.y, -m_halfSize.z ),
		Vector( m_halfSize.x, -m_halfSize.y, m_halfSize.z ),
		Vector( m_halfSize.x, -m_halfSize.y, -m_halfSize.z ),
		Vector( -m_halfSize.x, m_halfSize.y, m_halfSize.z ),
		Vector( -m_halfSize.x, m_halfSize.y, -m_halfSize.z ),
		Vector( -m_halfSize.x, -m_halfSize.y, m_halfSize.z ),
		Vector( -m_halfSize.x, -m_halfSize.y, -m_halfSize.z )
	};

	m_matTransform.TransformPosition( points, points, std::extent_v<decltype( points )> );
	return BoxSphereBounds( points, std::extent_v<decltype( points )> );
}

Collider OrientedBox::GetType() const
{
	return Collider::Obb;
}

Vector OrientedBox::GetAxisVector( uint32 i ) const
{
	return Vector( m_matTransform.m[i][0], m_matTransform.m[i][1], m_matTransform.m[i][2] );
}

OrientedBox::OrientedBox( const AxisAlignedBox& box )
{
	Vector centre = box.Centroid();

	m_halfSize = box.GetMax() - centre;
	m_matTransform = Matrix(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		centre.x, centre.y, centre.z, 1.f );

	m_matInvTransform = m_matTransform.Inverse();
}

OrientedBox::OrientedBox( const Vector& halfSize, const Matrix& transform )
	: m_halfSize( halfSize )
	, m_matTransform( transform )
	, m_matInvTransform( transform.Inverse() )
{}

bool TryAxis( const OrientedBox& lhs, const OrientedBox& rhs, const Vector& axis, const Vector& toCentre, uint32 index, float& smallestPenetration, uint32& smallestCase )
{
	if ( axis.LengthSqrt() < 0.0001f )
	{
		return true;
	}

	float penetration = CalcPenetrationOnAxis( lhs, rhs, axis, toCentre );

	if ( penetration < 0 )
	{
		return false;
	}
	if ( penetration < smallestPenetration )
	{
		smallestPenetration = penetration;
		smallestCase = index;
	}

	return true;
}
