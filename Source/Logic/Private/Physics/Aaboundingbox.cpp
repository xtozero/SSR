#include "stdafx.h"
#include "Physics/Aaboundingbox.h"

#include "Core/Timer.h"
#include "Model/CommonMeshDefine.h"
#include "Model/IMesh.h"
#include "Physics/CollideNarrow.h"
#include "Physics/Frustum.h"
#include "Physics/Ray.h"
#include "Scene/DebugOverlayManager.h"
#include "Util.h"

#include <algorithm> 

using namespace DirectX;

void CAaboundingbox::CalcMeshBounds( const IMesh& mesh )
{
	m_min = CXMFLOAT3( FLT_MAX, FLT_MAX, FLT_MAX );
	m_max = CXMFLOAT3( -FLT_MAX, -FLT_MAX, -FLT_MAX );

	int verticesCount = mesh.GetVerticesCount( );
	const MeshVertex* pVertices = static_cast<const MeshVertex*>( mesh.GetMeshData( ) );

	for ( int i = 0; i < verticesCount; ++i )
	{
		m_min.x = m_min.x > pVertices[i].m_position.x ? pVertices[i].m_position.x : m_min.x;
		m_min.y = m_min.y > pVertices[i].m_position.y ? pVertices[i].m_position.y : m_min.y;
		m_min.z = m_min.z > pVertices[i].m_position.z ? pVertices[i].m_position.z : m_min.z;
		m_max.x = m_max.x < pVertices[i].m_position.x ? pVertices[i].m_position.x : m_max.x;
		m_max.y = m_max.y < pVertices[i].m_position.y ? pVertices[i].m_position.y : m_max.y;
		m_max.z = m_max.z < pVertices[i].m_position.z ? pVertices[i].m_position.z : m_max.z;
	}

#ifdef TEST_CODE
	DebugMsg( "-------------------AABB-------------------\n" );
	DebugMsg( "max %f, %f, %f\n", m_max.x, m_max.y, m_max.z );
	DebugMsg( "min %f, %f, %f\n", m_min.x, m_min.y, m_min.z );
	DebugMsg( "------------------------------------------\n" );
#endif
}

void CAaboundingbox::Update( const CXMFLOAT3& scaling, const CXMFLOAT4& rotation, const CXMFLOAT3& translation, ICollider* original )
{
	CAaboundingbox* orig = dynamic_cast<CAaboundingbox*>( original );
	if ( orig == nullptr )
	{
		return;
	}

	CXMFLOAT3 v[8] = {
		CXMFLOAT3( orig->m_max.x, orig->m_max.y, orig->m_max.z ),
		CXMFLOAT3( orig->m_max.x, orig->m_min.y, orig->m_max.z ),
		CXMFLOAT3( orig->m_max.x, orig->m_max.y, orig->m_min.z ),
		CXMFLOAT3( orig->m_min.x, orig->m_max.y, orig->m_max.z ),
		CXMFLOAT3( orig->m_min.x, orig->m_max.y, orig->m_min.z ),
		CXMFLOAT3( orig->m_min.x, orig->m_min.y, orig->m_max.z ),
		CXMFLOAT3( orig->m_max.x, orig->m_min.y, orig->m_min.z ),
		CXMFLOAT3( orig->m_min.x, orig->m_min.y, orig->m_min.z ),
	};
	
	CXMFLOAT4X4 matrix = XMMatrixAffineTransformation( scaling, g_XMZero, rotation, translation );

	for ( int i = 0; i < 8; ++i )
	{
		v[i] = XMVector3TransformCoord( v[i], matrix );
	}

	m_min = CXMFLOAT3( FLT_MAX, FLT_MAX, FLT_MAX );
	m_max = CXMFLOAT3( -FLT_MAX, -FLT_MAX, -FLT_MAX );

	for ( int i = 0; i < 8; ++i )
	{
		m_min.x = m_min.x > v[i].x ? v[i].x : m_min.x;
		m_min.y = m_min.y > v[i].y ? v[i].y : m_min.y;
		m_min.z = m_min.z > v[i].z ? v[i].z : m_min.z;
		m_max.x = m_max.x < v[i].x ? v[i].x : m_max.x;
		m_max.y = m_max.y < v[i].y ? v[i].y : m_max.y;
		m_max.z = m_max.z < v[i].z ? v[i].z : m_max.z;
	}
}

void CAaboundingbox::CalcSubMeshBounds( std::vector<std::unique_ptr<ICollider>>& subColliders )
{
	subColliders.clear( );

	CXMFLOAT3 length = m_max - m_min;

	for ( int i = 0; i < 8 * 8 * 8; ++i )
	{
		float ix = static_cast<float>( i & 0x7 );
		float iy = static_cast<float>( ( i >> 3 ) & 0x7 );
		float iz = static_cast<float>( ( i >> 6 ) & 0x7 );

		CAaboundingbox* subBox = new CAaboundingbox;
		subBox->SetMin( m_min.x + ( ix / 8.f ) * length.x, m_min.y + ( iy / 8.f ) * length.y, m_min.z + ( iz / 8.f ) * length.z );
		subBox->SetMax( m_min.x + ( ( ix + 1 ) / 8.f ) * length.x, m_min.y + ( ( iy + 1 ) / 8.f ) * length.y, m_min.z + ( ( iz + 1 ) / 8.f ) * length.z );

		subColliders.emplace_back( subBox );
	}
}

float CAaboundingbox::Intersect( const CRay& ray ) const
{
	return RayAndBox( ray.GetOrigin( ), ray.GetDir( ), m_max, m_min );
}

int CAaboundingbox::Intersect( const CFrustum& frustum ) const
{
	using namespace DirectX;

	const CFrustum::LookUpTable& lut = frustum.GetVertexLUT( );
	const CXMFLOAT4( &planes )[6] = frustum.GetPlanes( );

	int result = COLLISION::INSIDE;
	for ( int i = 0; i < 6; ++i )
	{
		CXMFLOAT3 p( ( lut[i] & CFrustum::X_MAX ) ? m_max.x : m_min.x, ( lut[i] & CFrustum::Y_MAX ) ? m_max.y : m_min.y, ( lut[i] & CFrustum::Z_MAX ) ? m_max.z : m_min.z );
		CXMFLOAT3 n( ( lut[i] & CFrustum::X_MAX ) ? m_min.x : m_max.x, ( lut[i] & CFrustum::Y_MAX ) ? m_min.y : m_max.y, ( lut[i] & CFrustum::Z_MAX ) ? m_min.z : m_max.z );

		if ( XMVectorGetX( XMPlaneDotCoord( planes[i], p ) ) < 0 )
		{
			return COLLISION::OUTSIDE;
		}

		if ( XMVectorGetX( XMPlaneDotCoord( planes[i], n ) ) < 0 )
		{
			result = COLLISION::INTERSECTION;
		}
	}

	return result;
}

void CAaboundingbox::DrawDebugOverlay( CDebugOverlayManager& debugOverlay, unsigned int color, float duration ) const
{
	debugOverlay.AddDebugCube( m_min, m_max, color, duration );
}

int CAaboundingbox::Intersect( const CAaboundingbox& box ) const
{
	for ( int i = 0; i < 3; ++i )
	{
		if ( m_max[i] < box.m_min[i] || m_min[i] > box.m_max[i] )
		{
			return COLLISION::OUTSIDE;
		}
	}

	if ( m_min.x <= box.m_min.x && box.m_max.x <= m_max.x &&
		m_min.y <= box.m_min.y && box.m_max.y <= m_max.y &&
		m_min.z <= box.m_min.z && box.m_max.z <= m_max.z )
	{
		return COLLISION::INSIDE;
	}

	return COLLISION::INTERSECTION;
}

CAaboundingbox::CAaboundingbox( const std::vector<CAaboundingbox>& boxes )
{
	for ( const auto& box : boxes )
	{
		Merge( box.m_max );
		Merge( box.m_min );
	}
}

CAaboundingbox::CAaboundingbox( const std::vector<CXMFLOAT3>& points )
{
	for ( const auto& point : points )
	{
		Merge( point );
	}
}

void CAaboundingbox::Merge( const CXMFLOAT3& vec )
{
	m_max.x = std::max( m_max.x, vec.x );
	m_max.y = std::max( m_max.y, vec.y );
	m_max.z = std::max( m_max.z, vec.z );
	m_min.x = std::min( m_min.x, vec.x );
	m_min.y = std::min( m_min.y, vec.y );
	m_min.z = std::min( m_min.z, vec.z );
}

void TransformAABB( CAaboundingbox& result, const CAaboundingbox& src, const CXMFLOAT4X4& mat )
{
	CXMFLOAT3 point[8];
	for ( int i = 0; i < 8; ++i )
	{
		point[i] = src.Point( i );
	}

	result.Reset( );

	for ( int i = 0; i < 8; ++i )
	{
		point[i] = XMVector3TransformCoord( point[i], mat );
		result.Merge( point[i] );
	}
}