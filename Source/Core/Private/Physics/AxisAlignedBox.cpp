#include "AxisAlignedBox.h"

#include "CollideNarrow.h"
#include "Frustum.h"
#include "Math/TransformationMatrix.h"
#include "Ray.h"

#include <algorithm> 

void AxisAlignedBox::Update( const Vector& scaling, const Quaternion& rotation, const Vector& translation, ICollider* original )
{
	AxisAlignedBox* orig = dynamic_cast<AxisAlignedBox*>( original );
	if ( orig == nullptr )
	{
		return;
	}

	Vector v[8] = {
		Vector( orig->m_max.x, orig->m_max.y, orig->m_max.z ),
		Vector( orig->m_max.x, orig->m_min.y, orig->m_max.z ),
		Vector( orig->m_max.x, orig->m_max.y, orig->m_min.z ),
		Vector( orig->m_min.x, orig->m_max.y, orig->m_max.z ),
		Vector( orig->m_min.x, orig->m_max.y, orig->m_min.z ),
		Vector( orig->m_min.x, orig->m_min.y, orig->m_max.z ),
		Vector( orig->m_max.x, orig->m_min.y, orig->m_min.z ),
		Vector( orig->m_min.x, orig->m_min.y, orig->m_min.z ),
	};

	Matrix matrix = ScaleRotationTranslationMatrix( scaling, rotation, translation );

	for ( uint32 i = 0; i < 8; ++i )
	{
		v[i] = matrix.TransformPosition( v[i] );
	}

	m_min = Vector( FLT_MAX, FLT_MAX, FLT_MAX );
	m_max = Vector( -FLT_MAX, -FLT_MAX, -FLT_MAX );

	for ( uint32 i = 0; i < 8; ++i )
	{
		m_min.x = m_min.x > v[i].x ? v[i].x : m_min.x;
		m_min.y = m_min.y > v[i].y ? v[i].y : m_min.y;
		m_min.z = m_min.z > v[i].z ? v[i].z : m_min.z;
		m_max.x = m_max.x < v[i].x ? v[i].x : m_max.x;
		m_max.y = m_max.y < v[i].y ? v[i].y : m_max.y;
		m_max.z = m_max.z < v[i].z ? v[i].z : m_max.z;
	}
}

float AxisAlignedBox::Intersect( const CRay& ray ) const
{
	return RayAndBox( ray.GetOrigin(), ray.GetDir(), m_max, m_min );
}

CollisionResult AxisAlignedBox::Intersect( const Frustum& frustum ) const
{
	const Frustum::LookUpTable& lut = frustum.GetVertexLUT();
	const Plane( &planes )[6] = frustum.GetPlanes();

	CollisionResult result = CollisionResult::Inside;
	for ( uint32 i = 0; i < 6; ++i )
	{
		Vector p( ( lut[i] & Frustum::X_MAX ) ? m_max.x : m_min.x, ( lut[i] & Frustum::Y_MAX ) ? m_max.y : m_min.y, ( lut[i] & Frustum::Z_MAX ) ? m_max.z : m_min.z );
		Vector n( ( lut[i] & Frustum::X_MAX ) ? m_min.x : m_max.x, ( lut[i] & Frustum::Y_MAX ) ? m_min.y : m_max.y, ( lut[i] & Frustum::Z_MAX ) ? m_min.z : m_max.z );

		if ( planes[i].PlaneDot( p ) < 0 )
		{
			return CollisionResult::Outside;
		}

		if ( planes[i].PlaneDot( n ) < 0 )
		{
			result = CollisionResult::Intersection;
		}
	}

	return result;
}

BoxSphereBounds AxisAlignedBox::Bounds() const
{
	Vector points[] = {
		m_min,
		m_max,
	};

	return BoxSphereBounds( points, std::extent_v<decltype( points )> );
}

Collider AxisAlignedBox::GetType() const
{
	return Collider::Aabb;
}

CollisionResult AxisAlignedBox::Intersect( const AxisAlignedBox& box ) const
{
	for ( uint32 i = 0; i < 3; ++i )
	{
		if ( m_max[i] < box.m_min[i] || m_min[i] > box.m_max[i] )
		{
			return CollisionResult::Outside;
		}
	}

	if ( m_min.x <= box.m_min.x && box.m_max.x <= m_max.x &&
		m_min.y <= box.m_min.y && box.m_max.y <= m_max.y &&
		m_min.z <= box.m_min.z && box.m_max.z <= m_max.z )
	{
		return CollisionResult::Inside;
	}

	return CollisionResult::Intersection;
}

AxisAlignedBox::AxisAlignedBox( const std::vector<AxisAlignedBox>& boxes )
{
	for ( const auto& box : boxes )
	{
		Merge( box.m_max );
		Merge( box.m_min );
	}
}

AxisAlignedBox::AxisAlignedBox( const BoxSphereBounds& bounds )
{
	Merge( bounds.Origin() - bounds.HalfSize() );
	Merge( bounds.Origin() + bounds.HalfSize() );
}

AxisAlignedBox::AxisAlignedBox( const Vector* points, uint32 numPoints )
{
	for ( uint32 i = 0; i < numPoints; ++i )
	{
		Merge( points[i] );
	}
}

void AxisAlignedBox::Merge( const Vector& vec )
{
	m_max.x = std::max( m_max.x, vec.x );
	m_max.y = std::max( m_max.y, vec.y );
	m_max.z = std::max( m_max.z, vec.z );
	m_min.x = std::min( m_min.x, vec.x );
	m_min.y = std::min( m_min.y, vec.y );
	m_min.z = std::min( m_min.z, vec.z );
}

void TransformAABB( AxisAlignedBox& result, const AxisAlignedBox& src, const Matrix& mat )
{
	Vector point[8];
	for ( uint32 i = 0; i < 8; ++i )
	{
		point[i] = src.Point( i );
	}

	result.Reset();

	for ( uint32 i = 0; i < 8; ++i )
	{
		point[i] = mat.TransformPosition( point[i] );
		result.Merge( point[i] );
	}
}