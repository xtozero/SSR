#include "stdafx.h"
#include "BoundingSphere.h"

#include "Model/CommonMeshDefine.h"
#include "Model/IMesh.h"
#include "Ray.h"

using namespace DirectX;

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