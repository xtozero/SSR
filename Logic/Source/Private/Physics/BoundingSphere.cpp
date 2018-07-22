#include "stdafx.h"
#include "Physics/BoundingSphere.h"

#include "Model/CommonMeshDefine.h"
#include "Model/IMesh.h"
#include "Physics/Aaboundingbox.h"
#include "Physics/Frustum.h"
#include "Physics/Ray.h"

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
			t0 = min( tmp0, tmp1 );
			t1 = max( tmp0, tmp1 );
			return true;
		}
	}
}

void BoundingSphere::CreateRigideBody( const IMesh& mesh )
{
	int verticesCount = mesh.GetVerticesCount( );
	const MeshVertex* pVertices = static_cast<const MeshVertex*>( mesh.GetMeshData( ) );

	m_radiusSqr = -FLT_MAX;
	for ( int i = 0; i < verticesCount; ++i )
	{
		float radiusSqr = XMVectorGetX( XMVector3LengthSq( pVertices[i].m_position ) );

		if ( radiusSqr > m_radiusSqr )
		{
			m_radiusSqr = radiusSqr;
		}
	}
}

void BoundingSphere::Update( const CXMFLOAT4X4& matrix, IRigidBody* original )
{
	BoundingSphere* orig = dynamic_cast<BoundingSphere*>( original );
	
	m_origin = CXMFLOAT3( matrix._41, matrix._42, matrix._43 );

	float scale = max( matrix._11, max( matrix._22, max( matrix._33, 1 ) ) );
	float newRadius = sqrt( orig->m_radiusSqr ) * scale;
	m_radiusSqr = newRadius * newRadius;
}

float BoundingSphere::Intersect( const CRay* ray ) const
{
	XMVECTOR toShpere = m_origin - ray->GetOrigin( );

	float toShpereSqr = XMVectorGetX( XMVector3LengthSq( toShpere ) );
	float tangentSqr = XMVectorGetX( XMVector3Dot( toShpere, ray->GetDir( ) ) );
	tangentSqr *= tangentSqr;

	float normalVectorSqr = toShpereSqr - tangentSqr;

	if ( normalVectorSqr > m_radiusSqr )
	{
		return -1;
	}

	return max( 0.f, std::sqrt( tangentSqr ) - std::sqrt( m_radiusSqr - normalVectorSqr ) );
}

int BoundingSphere::Intersect( const CFrustum& frustum ) const
{
	const CXMFLOAT4( &planes )[6] = frustum.GetPlanes( );

	bool inside = true;
	float radius = sqrtf( m_radiusSqr );

	for ( int i = 0; ( i<6 ) && inside; i++ )
		inside &= ( ( XMVectorGetX( XMPlaneDotNormal( planes[i], m_origin ) ) + radius ) >= 0.f );

	return inside;
}

bool BoundingSphere::Intersect( const CFrustum& frustum, const CXMFLOAT3& sweepDir )
{
	float displacement[12];
	int count = 0;
	float t0 = -1;
	float t1 = -1;
	bool inFrustum = false;

	const CXMFLOAT4(&planes)[6] = frustum.GetPlanes( );

	for ( int i = 0; i < 6; ++i )
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

	for ( int i = 0; i < count; ++i )
	{
		float radius = m_radiusSqr * 1.1f * 1.1f;
		CXMFLOAT3 center( m_origin );
		center += sweepDir * displacement[i];
		BoundingSphere sphere( center, radius );
		inFrustum |= ( sphere.Intersect( frustum ) > 0 );
	}

	return inFrustum;
}

BoundingSphere::BoundingSphere( const CAaboundingbox& box )
{
	box.Centroid( m_origin );
	m_radiusSqr = XMVectorGetX( XMVector3LengthSq( m_origin - box.GetMax( ) ) );
}

BoundingSphere::BoundingSphere( const std::vector<CXMFLOAT3>& points )
{
	auto iter = points.begin( );

	float radius = 0.f;
	m_origin = *iter++;

	while ( iter != points.end( ) )
	{
		const CXMFLOAT3 tmp = *iter++;
		CXMFLOAT3 toPoint = tmp - m_origin;
		float d = XMVectorGetX( XMVector3Dot( toPoint, toPoint ) );
		if ( d > radius * radius )
		{
			d = sqrtf( d );
			float r = 0.5f * ( d + radius );
			float scale = ( r - radius ) / d;
			m_origin += scale * toPoint;
			radius = r;
		}
	}

	m_radiusSqr = radius * radius;
}
