#include "stdafx.h"
#include "Physics/OrientedBoundingBox.h"

#include "Core/Timer.h"
//#include "Model/CommonMeshDefine.h"
#include "Physics/Aaboundingbox.h"
#include "Physics/CollideNarrow.h"
#include "Physics/Frustum.h"
#include "Physics/Ray.h"
#include "Scene/DebugOverlayManager.h"
#include "Util.h"

using namespace DirectX;

namespace
{
	float TransformToAxis( const COrientedBoundingBox& box, const CXMFLOAT3& axis )
	{
		const CXMFLOAT3& halfSize = box.GetHalfSize( );

		return halfSize.x * fabsf( XMVectorGetX( XMVector3Dot( axis, box.GetAxisVector( 0 ) ) ) ) +
			halfSize.y * fabsf( XMVectorGetX( XMVector3Dot( axis, box.GetAxisVector( 1 ) ) ) ) +
			halfSize.z * fabsf( XMVectorGetX( XMVector3Dot( axis, box.GetAxisVector( 2 ) ) ) );
	}

	bool OverlapOnAxis( const COrientedBoundingBox& lhs, const COrientedBoundingBox& rhs, const CXMFLOAT3& axis, const CXMFLOAT3& toCentre )
	{
		return CalcPenetrationOnAxis( lhs, rhs, axis, toCentre ) > 0;
	}
}

void COrientedBoundingBox::CalcMeshBounds( const MeshData& mesh )
{
	//int verticesCount = mesh.m_vertices;
	//const MeshVertex* pVertices = static_cast<const MeshVertex*>( mesh.m_pVertexData );

	//CXMFLOAT3 length;
	//for ( int i = 0; i < verticesCount; ++i )
	//{
	//	for ( int j = 0; j < 3; ++j )
	//	{
	//		length[j] = fabsf( pVertices[i].m_position[j] );
	//		if ( length[j] > m_halfSize[j] )
	//		{
	//			m_halfSize[j] = length[j];
	//		}
	//	}
	//}
}

void COrientedBoundingBox::Update( const CXMFLOAT3& scaling, const CXMFLOAT4& rotation, const CXMFLOAT3& translation, ICollider* original )
{
	COrientedBoundingBox* orig = dynamic_cast<COrientedBoundingBox*>( original );
	if ( orig == nullptr )
	{
		return;
	}

	m_matTransform = XMMatrixAffineTransformation( g_XMOne3, g_XMZero, rotation, translation );
	m_matInvTransform = XMMatrixInverse( nullptr, m_matTransform );

	for ( int i = 0; i < 3; ++i )
	{
		m_halfSize[i] = orig->m_halfSize[i] * scaling[i];
	}
}

void COrientedBoundingBox::CalcSubMeshBounds( std::vector<std::unique_ptr<ICollider>>& /*subColliders*/ )
{
}

float COrientedBoundingBox::Intersect( const CRay& ray ) const
{
	CXMFLOAT3 rayOrigin = XMVector3TransformCoord( ray.GetOrigin(), m_matInvTransform );
	CXMFLOAT3 rayDir = XMVector3TransformNormal( ray.GetDir( ), m_matInvTransform );

	return RayAndBox( rayOrigin, rayDir, m_halfSize, -m_halfSize );
}

int COrientedBoundingBox::Intersect( const CFrustum& /*frustum*/ ) const
{
	return 0;
}

void COrientedBoundingBox::DrawDebugOverlay( CDebugOverlayManager& debugOverlay, unsigned int color, float duration ) const
{
	debugOverlay.AddDebugCube( m_halfSize, m_matTransform, color, duration );
}

CXMFLOAT3 COrientedBoundingBox::GetAxisVector( int i ) const
{
	return CXMFLOAT3( m_matTransform.m[i][0], m_matTransform.m[i][1], m_matTransform.m[i][2] );
}

COrientedBoundingBox::COrientedBoundingBox( const CAaboundingbox& box )
{
	CXMFLOAT3 centre;
	box.Centroid( centre );

	m_halfSize = box.GetMax( ) - centre;
	m_matTransform = CXMFLOAT4X4( 
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		centre.x, centre.y, centre.z, 1.f );

	m_matInvTransform = XMMatrixInverse( nullptr, m_matTransform );
}

float CalcPenetrationOnAxis( const COrientedBoundingBox& lhs, const COrientedBoundingBox& rhs, const CXMFLOAT3& axis, const CXMFLOAT3& toCentre )
{
	float lhsProject = TransformToAxis( lhs, axis );
	float rhsProject = TransformToAxis( rhs, axis );

	float distance = fabsf( XMVectorGetX( XMVector3Dot( toCentre, axis ) ) );

	return lhsProject + rhsProject - distance;
}

bool TryAxis( const COrientedBoundingBox& lhs, const COrientedBoundingBox& rhs, const CXMFLOAT3& axis, const CXMFLOAT3& toCentre, unsigned int index, float& smallestPenetration, unsigned int& smallestCase )
{
	if ( XMVectorGetX( XMVector3LengthSq( axis ) ) < 0.0001 )
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
