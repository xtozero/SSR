#include "BoundingSphere.h"

#include "AxisAlignedBox.h"
#include "CollideNarrow.h"
#include "Frustum.h"
#include "OrientedBox.h"
#include "Ray.h"

#include <algorithm>

namespace
{
	bool SweptSpherePlaneIntersection( float& t0, float& t1, const Plane& plane, const BoundingSphere& sphere, const Vector& sweepDir )
	{
		float bdotn = plane.PlaneDot( sphere.GetCenter() );
		float ddotn = plane.PlaneDotNormal( sweepDir );

		float radius = sphere.GetRadius();

		if ( ddotn == 0 )
		{
			if ( bdotn <= radius )
			{
				t0 = 0;
				t1 = 1e32f;
				return true;
			}

			return false;
		}
		else
		{
			float tmp0 = ( radius - bdotn ) / ddotn;
			float tmp1 = ( -radius - bdotn ) / ddotn;
			t0 = std::min( tmp0, tmp1 );
			t1 = std::max( tmp0, tmp1 );
			return true;
		}
	}
}

void BoundingSphere::Update( const Vector& scaling, const Quaternion& /*rotation*/, const Vector& translation, ICollider* original )
{
	BoundingSphere* orig = dynamic_cast<BoundingSphere*>( original );
	m_origin = translation;

	float maxScaling = std::max( scaling.x, std::max( scaling.y, std::max( scaling.z, 1.f ) ) );
	m_radius = orig->GetRadius() * maxScaling;
}

float BoundingSphere::Intersect( const CRay& ray ) const
{
	return RayAndSphere( ray.GetOrigin(), ray.GetDir(), m_origin, m_radius );
}

CollisionResult BoundingSphere::Intersect( const Frustum& frustum ) const
{
	const Plane( &planes )[6] = frustum.GetPlanes();

	bool intersection = true;

	for ( uint32 i = 0; ( i < 6 ) && intersection; i++ )
	{
		intersection = intersection && ( ( planes[i].PlaneDot( m_origin ) + m_radius ) >= 0.f );
	}

	return intersection ? CollisionResult::Intersection : CollisionResult::Outside;
}

BoxSphereBounds BoundingSphere::Bounds() const
{
	return BoxSphereBounds( m_origin, Vector( m_radius, m_radius, m_radius ), m_radius );
}

Collider BoundingSphere::GetType() const
{
	return Collider::Sphere;
}

CollisionResult BoundingSphere::Intersect( const BoundingSphere& sphere ) const
{
	float distance = ( m_origin - sphere.m_origin ).LengthSqrt();

	return ( distance <= ( m_radius + sphere.m_radius ) * ( m_radius + sphere.m_radius ) ) ? CollisionResult::Intersection : CollisionResult::Outside;
}

float BoundingSphere::CalcGrowth( const BoundingSphere& sphere ) const
{
	BoundingSphere newSphere( *this, sphere );

	float radiusDiff = ( newSphere.GetRadius() - m_radius );

	// ( ( 4 / 3 ) * pi * r^3 )
	return 1.33333f * DirectX::XM_PI * radiusDiff * radiusDiff * radiusDiff;
}

bool BoundingSphere::Intersect( const Frustum& frustum, const Vector& sweepDir )
{
	float displacement[12];
	uint32 count = 0;
	float t0 = -1;
	float t1 = -1;
	bool inFrustum = false;

	const Plane( &planes )[6] = frustum.GetPlanes();

	for ( uint32 i = 0; i < 6; ++i )
	{
		if ( SweptSpherePlaneIntersection( t0, t1, planes[i], *this, sweepDir ) )
		{
			if ( t0 >= 0.f )
			{
				displacement[count++] = t0;
			}
			if ( t1 >= 0.f )
			{
				displacement[count++] = t1;
			}
		}
	}

	for ( uint32 i = 0; i < count; ++i )
	{
		float radius = m_radius * 1.1f;
		Vector center( m_origin );
		center += sweepDir * displacement[i];
		BoundingSphere sphere( center, radius );
		inFrustum |= ( sphere.Intersect( frustum ) > CollisionResult::Outside );
	}

	return inFrustum;
}

BoundingSphere::BoundingSphere( const AxisAlignedBox& box )
{
	m_origin = box.Centroid();
	m_radius = ( m_origin - box.GetMax() ).Length();
}

BoundingSphere::BoundingSphere( const OrientedBox& box )
{
	m_origin = box.GetAxisVector( 3 );
	m_radius = box.GetHalfSize().Length();
}

BoundingSphere::BoundingSphere( const Vector* points, size_t count )
{
	if ( count > 1 )
	{
		m_origin = points[0];

		for ( size_t i = 1; i < count; ++i )
		{
			const Vector& tmp = points[i];
			Vector toPoint = tmp - m_origin;
			float d = toPoint | toPoint;
			if ( d > m_radius * m_radius )
			{
				d = sqrtf( d );
				float r = 0.5f * ( d + m_radius );
				float scale = ( r - m_radius ) / d;
				m_origin += scale * toPoint;
				m_radius = r;
			}
		}
	}
}

BoundingSphere::BoundingSphere( const BoundingSphere& one, const BoundingSphere& two )
{
	Vector centerOffset = two.m_origin - one.m_origin;
	float distance = centerOffset.LengthSqrt();

	float radiusDiff = two.m_radius - one.m_radius;

	if ( radiusDiff * radiusDiff >= distance )
	{
		if ( one.m_radius > two.m_radius )
		{
			m_origin = one.m_origin;
			m_radius = one.m_radius;
		}
		else
		{
			m_origin = two.m_origin;
			m_radius = two.m_radius;
		}
	}
	else
	{
		distance = sqrtf( distance );
		m_radius = ( distance + one.m_radius + two.m_radius ) * 0.5f;

		m_origin = one.m_origin;
		if ( distance > 0 )
		{
			m_origin += centerOffset * ( ( m_radius - one.m_radius ) / distance );
		}
	}
}
