#include "stdafx.h"
#include "Physics/BoundingSphere.h"

#include "Core/Timer.h"
//#include "Model/CommonMeshDefine.h"
#include "Physics/Aaboundingbox.h"
#include "Physics/CollideNarrow.h"
#include "Physics/Frustum.h"
#include "Physics/OrientedBoundingBox.h"
#include "Physics/Ray.h"
#include "Scene/DebugOverlayManager.h"

#include <algorithm>

using namespace DirectX;

namespace
{
	bool SweptSpherePlaneIntersection( float& t0, float& t1, const CXMFLOAT4& plane, const BoundingSphere& sphere, const CXMFLOAT3& sweepDir )
	{
		float bdotn = XMVectorGetX( XMPlaneDotCoord( plane, sphere.GetCenter() ) );
		float ddotn = XMVectorGetX( XMPlaneDotNormal( plane, sweepDir ) );

		float radius = sphere.GetRadius( );

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

void BoundingSphere::CalcMeshBounds( const MeshData& mesh )
{
	//uint32 verticesCount = mesh.m_vertices;
	//const MeshVertex* pVertices = static_cast<const MeshVertex*>( mesh.m_pVertexData );

	//float maxRadiusSqr = -FLT_MAX;
	//for ( uint32 i = 0; i < verticesCount; ++i )
	//{
	//	float radiusSqr = XMVectorGetX( XMVector3LengthSq( pVertices[i].m_position ) );

	//	if ( radiusSqr > maxRadiusSqr )
	//	{
	//		maxRadiusSqr = radiusSqr;
	//	}
	//}

	//m_radius = sqrtf( maxRadiusSqr );
}

void BoundingSphere::Update( const CXMFLOAT3& scaling, const CXMFLOAT4& /*rotation*/, const CXMFLOAT3& translation, ICollider* original )
{
	BoundingSphere* orig = dynamic_cast<BoundingSphere*>( original );
	m_origin = translation;

	float maxScaling = std::max( scaling.x, std::max( scaling.y, std::max( scaling.z, 1.f ) ) );
	m_radius = orig->GetRadius() * maxScaling;
}

float BoundingSphere::Intersect( const CRay& ray ) const
{
	return RayAndSphere( ray.GetOrigin( ), ray.GetDir( ), m_origin, m_radius );
}

uint32 BoundingSphere::Intersect( const CFrustum& frustum ) const
{
	const CXMFLOAT4( &planes )[6] = frustum.GetPlanes( );

	bool inside = true;

	for ( uint32 i = 0; ( i<6 ) && inside; i++ )
		inside &= ( ( XMVectorGetX( XMPlaneDotCoord( planes[i], m_origin ) ) + m_radius ) >= 0.f );

	return inside;
}

void BoundingSphere::DrawDebugOverlay( CDebugOverlayManager& debugOverlay, uint32 color, float duration ) const
{
	debugOverlay.AddDebugSphere( m_origin, m_radius, color, duration );
}

uint32 BoundingSphere::Intersect( const BoundingSphere& sphere ) const
{
	float distance = XMVectorGetX( XMVector3LengthSq( m_origin - sphere.m_origin ) );

	return ( distance < ( m_radius + sphere.m_radius ) * ( m_radius + sphere.m_radius ) ) ? 1 : 0;
}

float BoundingSphere::CalcGrowth( const BoundingSphere& sphere ) const
{
	BoundingSphere newSphere( *this, sphere );

	float radiusDiff = ( newSphere.GetRadius( ) - m_radius );

	// ( ( 4 / 3 ) * pi * r^3 )
	return 1.33333f * XM_PI * radiusDiff * radiusDiff * radiusDiff;
}

bool BoundingSphere::Intersect( const CFrustum& frustum, const CXMFLOAT3& sweepDir )
{
	float displacement[12];
	uint32 count = 0;
	float t0 = -1;
	float t1 = -1;
	bool inFrustum = false;

	const CXMFLOAT4(&planes)[6] = frustum.GetPlanes( );

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
		CXMFLOAT3 center( m_origin );
		center += sweepDir * displacement[i];
		BoundingSphere sphere( center, radius );
		inFrustum |= ( sphere.Intersect( frustum ) > COLLISION::OUTSIDE );
	}

	return inFrustum;
}

BoundingSphere::BoundingSphere( const CAaboundingbox& box )
{
	box.Centroid( m_origin );
	m_radius = XMVectorGetX( XMVector3Length( m_origin - box.GetMax( ) ) );
}

BoundingSphere::BoundingSphere( const COrientedBoundingBox& box )
{
	m_origin = box.GetAxisVector( 3 );
	m_radius = XMVectorGetX( XMVector3Length( box.GetHalfSize( ) ) );
}

BoundingSphere::BoundingSphere( const std::vector<CXMFLOAT3>& points )
{
	auto iter = points.begin( );

	m_radius = 0.f;
	m_origin = *iter++;

	while ( iter != points.end( ) )
	{
		const CXMFLOAT3 tmp = *iter++;
		CXMFLOAT3 toPoint = tmp - m_origin;
		float d = XMVectorGetX( XMVector3Dot( toPoint, toPoint ) );
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

BoundingSphere::BoundingSphere( const BoundingSphere& one, const BoundingSphere& two )
{
	CXMFLOAT3 centerOffset = two.m_origin - one.m_origin;
	float distance = XMVectorGetX( XMVector3LengthSq( centerOffset ) );

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
