#include "stdafx.h"
#include "Physics/Aaboundingbox.h"

#include "Model/CommonMeshDefine.h"
#include "Model/IMesh.h"
#include "Physics/Frustum.h"
#include "Physics/Ray.h"
#include "Util.h"

#include <algorithm> 

using namespace DirectX;

void CAaboundingbox::CreateRigideBody( const IMesh& mesh )
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

void CAaboundingbox::Update( const CXMFLOAT4X4& matrix, IRigidBody* original )
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

void CAaboundingbox::CalcSubRigidBody( std::vector<std::unique_ptr<IRigidBody>>& subRigidBody )
{
	subRigidBody.clear( );

	CXMFLOAT3 length = m_max - m_min;

	for ( int i = 0; i < 8 * 8 * 8; ++i )
	{
		float ix = static_cast<float>( i & 0x07 );
		float iy = static_cast<float>( ( i >> 3 ) & 0x07 );
		float iz = static_cast<float>( ( i >> 6 ) & 0x07 );

		CAaboundingbox* subBox = new CAaboundingbox;
		subBox->SetMin( m_min.x + ( ix / 8.f ) * length.x, m_min.y + ( iy / 8.f ) * length.y, m_min.z + ( iz / 8.f ) * length.z );
		subBox->SetMax( m_min.x + ( ( ix + 1 ) / 8.f ) * length.x, m_min.y + ( ( iy + 1 ) / 8.f ) * length.y, m_min.z + ( ( iz + 1 ) / 8.f ) * length.z );

		subRigidBody.emplace_back( subBox );
	}
}

float CAaboundingbox::Intersect( const CRay* ray ) const
{
	if ( ray == nullptr )
	{
		return -1.f;
	}

	float t_min = 0;
	float t_max = FLT_MAX;

	float dir[3] = { ray->GetDir( ).x, ray->GetDir( ).y, ray->GetDir( ).z };
	float origin[3] = { ray->GetOrigin( ).x, ray->GetOrigin( ).y, ray->GetOrigin( ).z };
	float max[3] = { m_max.x, m_max.y, m_max.z };
	float min[3] = { m_min.x, m_min.y, m_min.z };

	for ( int i = 0; i < 3; ++i )
	{
		if ( abs( dir[i] ) < FLT_EPSILON )
		{
			if ( origin[i] < min[i] || origin[i] > max[i] )
			{
				return -1.f;
			}
		}
		else
		{
			float d = 1.0f / dir[i];
			float t1 = ( min[i] - origin[i] ) * d;
			float t2 = ( max[i] - origin[i] ) * d;

			if ( t1 > t2 )
			{
				std::swap( t1, t2 );
			}

			t_min = max( t1, t_min );
			t_max = min( t2, t_max );

			if ( t_min > t_max )
			{
				return -1.f;
			}
		}
	}

	return t_min;
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
	m_max.x = max( m_max.x, vec.x );
	m_max.y = max( m_max.y, vec.y );
	m_max.z = max( m_max.z, vec.z );
	m_min.x = min( m_min.x, vec.x );
	m_min.y = min( m_min.y, vec.y );
	m_min.z = min( m_min.z, vec.z );
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