#include "BoxSphereBounds.h"

CXMFLOAT3& BoxSphereBounds::Origin( )
{
	return m_origin;
}

const CXMFLOAT3& BoxSphereBounds::Origin( ) const
{
	return m_origin;
}

CXMFLOAT3& BoxSphereBounds::HalfSize( )
{
	return m_halfSize;
}

const CXMFLOAT3& BoxSphereBounds::HalfSize( ) const
{
	return m_halfSize;
}

float& BoxSphereBounds::Radius( )
{
	return m_radius;
}

float BoxSphereBounds::Radius( ) const
{
	return m_radius;
}

BoxSphereBounds BoxSphereBounds::TransformBy( const CXMFLOAT4X4& m ) const
{
	using namespace DirectX;

	CXMFLOAT3 newOrigin = XMVector3TransformCoord( m_origin, m );

	CXMFLOAT3 halfSizeX( m_halfSize.x, m_halfSize.x, m_halfSize.x );
	CXMFLOAT3 halfSizeY( m_halfSize.y, m_halfSize.y, m_halfSize.y );
	CXMFLOAT3 halfSizeZ( m_halfSize.z, m_halfSize.z, m_halfSize.z );

	CXMFLOAT3 m0( m._11, m._12, m._13 );
	CXMFLOAT3 m1( m._21, m._22, m._23 );
	CXMFLOAT3 m2( m._31, m._32, m._33 );

	CXMFLOAT3 newHalfSize = XMVectorAbs( halfSizeX * m0 ) +
							XMVectorAbs( halfSizeY * m1 ) +
							XMVectorAbs( halfSizeZ * m2 );

	float newRadius = XMVectorGetX( XMVector3Length( newHalfSize ) );
	return BoxSphereBounds( newOrigin, newHalfSize, newRadius );
}