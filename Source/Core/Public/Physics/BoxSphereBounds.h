#pragma once

#include "Archive.h"
#include "ArchiveUtility.h"
#include "Aaboundingbox.h"
#include "BoundingSphere.h"

class BoxSphereBounds
{
public:
	CXMFLOAT3& Origin( );
	const CXMFLOAT3& Origin( ) const;

	CXMFLOAT3& HalfSize( );
	const CXMFLOAT3& HalfSize( ) const;

	float& Radius( );
	float Radius( ) const;

	BoxSphereBounds TransformBy( const CXMFLOAT4X4& m ) const;

	BoxSphereBounds( ) = default;
	BoxSphereBounds( const CXMFLOAT3& origin, const CXMFLOAT3& halfSize, float radius ) : m_origin( origin ), m_halfSize( halfSize ), m_radius( radius ) {}
	BoxSphereBounds( const CXMFLOAT3* points, int32 numPoints )
	{
		CAaboundingbox box( points, numPoints );

		m_halfSize = box.Size( ) * 0.5;
		m_origin = box.GetMin( ) + m_halfSize;

		m_radius = DirectX::XMVectorGetX( DirectX::XMVector3Length( m_halfSize ) );
	}

	friend Archive& operator<<( Archive& ar, BoxSphereBounds& bounds )
	{
		ar << bounds.m_origin << bounds.m_halfSize << bounds.m_radius;
		return ar;
	}

private:
	CXMFLOAT3 m_origin;
	CXMFLOAT3 m_halfSize;
	float m_radius = 0.f;
};
