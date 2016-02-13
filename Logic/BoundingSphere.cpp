#include "stdafx.h"
#include "BoundingSphere.h"
#include "Ray.h"
#include "../RenderCore/CommonMeshDefine.h"
#include "../RenderCore/IMesh.h"

void BoundingSphere::CreateRigideBody( std::shared_ptr<IMesh> pMesh )
{
	int verticesCount = pMesh->GetVerticesCount( );
	MeshVertex* pVertices = static_cast<MeshVertex*>( pMesh->GetMeshData( ) );

	m_radiusSqr = -FLT_MAX;
	for ( int i = 0; i < verticesCount; ++i )
	{
		float radiusSqr = D3DXVec3LengthSq( &pVertices[i].m_position );

		if ( radiusSqr > m_radiusSqr )
		{
			m_radiusSqr = radiusSqr;
		}
	}
}

void BoundingSphere::Update( const D3DXMATRIX& matrix, std::shared_ptr<IRigidBody> original )
{
	BoundingSphere* orig = dynamic_cast<BoundingSphere*>( original.get( ) );
	
	m_origin = D3DXVECTOR3( matrix._41, matrix._42, matrix._43 );

	float scale = max( matrix._11, max( matrix._22, max( matrix._33, 1 ) ) );
	float newRadius = sqrt( orig->m_radiusSqr ) * scale;
	m_radiusSqr = newRadius * newRadius;
}

float BoundingSphere::Intersect( const CRay* ray ) const
{
	D3DXVECTOR3& toShpere = m_origin - ray->GetOrigin( );

	float toShpereSqr = D3DXVec3LengthSq( &toShpere );
	float tangentSqr = D3DXVec3Dot( &toShpere, &ray->GetDir( ) );
	tangentSqr *= tangentSqr;

	float normalVectorSqr = toShpereSqr - tangentSqr;

	if ( normalVectorSqr > m_radiusSqr )
	{
		return -1;
	}

	return max( 0.f, std::sqrt( tangentSqr ) - std::sqrt( m_radiusSqr - normalVectorSqr ) );
}

BoundingSphere::BoundingSphere( ) :
m_origin( 0.f, 0.f, 0.f ),
m_radiusSqr( 0.f )
{
}