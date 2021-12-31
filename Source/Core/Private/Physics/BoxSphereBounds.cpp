#include "BoxSphereBounds.h"

Vector& BoxSphereBounds::Origin( )
{
	return m_origin;
}

const Vector& BoxSphereBounds::Origin( ) const
{
	return m_origin;
}

Vector& BoxSphereBounds::HalfSize( )
{
	return m_halfSize;
}

const Vector& BoxSphereBounds::HalfSize( ) const
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

BoxSphereBounds BoxSphereBounds::TransformBy( const Matrix& m ) const
{
	using namespace DirectX;

	Vector newOrigin = m.TransformPosition( m_origin );
	
	Vector halfSizeX( m_halfSize.x, m_halfSize.x, m_halfSize.x );
	Vector halfSizeY( m_halfSize.y, m_halfSize.y, m_halfSize.y );
	Vector halfSizeZ( m_halfSize.z, m_halfSize.z, m_halfSize.z );

	Vector m0( m._11, m._12, m._13 );
	Vector m1( m._21, m._22, m._23 );
	Vector m2( m._31, m._32, m._33 );

	Vector newHalfSize = ( halfSizeX * m0 ).GetAbs() + ( halfSizeY * m1 ).GetAbs() + ( halfSizeZ * m2 ).GetAbs();

	float newRadius = newHalfSize.Length( );
	return BoxSphereBounds( newOrigin, newHalfSize, newRadius );
}