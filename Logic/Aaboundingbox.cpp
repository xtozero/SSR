#include "stdafx.h"
#include "Aaboundingbox.h"

#include "../RenderCore/CommonMeshDefine.h"
#include "../RenderCore/IMesh.h"
#include "../Shared/Util.h"
#include "Ray.h"

#include <algorithm> 

void CAaboundingbox::CreateRigideBody( std::shared_ptr<IMesh> pMesh )
{
	m_min = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
	m_max = D3DXVECTOR3( -FLT_MAX, -FLT_MAX, -FLT_MAX );

	int verticesCount = pMesh->GetVerticesCount( );
	MeshVertex* pVertices = static_cast<MeshVertex*>( pMesh->GetMeshData( ) );

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
	DebugMsg( _T( "-------------------AABB-------------------\n" ) );
	DebugMsg( _T( "max %f, %f, %f\n" ), m_max.x, m_max.y, m_max.z );
	DebugMsg( _T( "min %f, %f, %f\n" ), m_min.x, m_min.y, m_min.z );
	DebugMsg( _T( "------------------------------------------\n" ) );
#endif
}

void CAaboundingbox::Update( const D3DXMATRIX& matrix, std::shared_ptr<IRigidBody> original )
{
	CAaboundingbox* orig = dynamic_cast<CAaboundingbox*>( original.get( ) );

	if ( orig == nullptr )
	{
		return;
	}

	D3DXVECTOR3 v[8] = {
		D3DXVECTOR3( orig->m_max.x, orig->m_max.y, orig->m_max.z ),
		D3DXVECTOR3( orig->m_max.x, orig->m_min.y, orig->m_max.z ),
		D3DXVECTOR3( orig->m_max.x, orig->m_max.y, orig->m_min.z ),
		D3DXVECTOR3( orig->m_min.x, orig->m_max.y, orig->m_max.z ),
		D3DXVECTOR3( orig->m_min.x, orig->m_max.y, orig->m_min.z ),
		D3DXVECTOR3( orig->m_min.x, orig->m_min.y, orig->m_max.z ),
		D3DXVECTOR3( orig->m_max.x, orig->m_min.y, orig->m_min.z ),
		D3DXVECTOR3( orig->m_min.x, orig->m_min.y, orig->m_min.z ),
	};

	for ( int i = 0; i < 8; ++i )
	{
		D3DXVec3TransformCoord( &v[i], &v[i], &matrix );
	}

	m_min = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
	m_max = D3DXVECTOR3( -FLT_MAX, -FLT_MAX, -FLT_MAX );

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

CAaboundingbox::CAaboundingbox( ) : m_max( -FLT_MAX, -FLT_MAX, -FLT_MAX ),
m_min( FLT_MAX, FLT_MAX, FLT_MAX )
{
}


CAaboundingbox::~CAaboundingbox( )
{
}
