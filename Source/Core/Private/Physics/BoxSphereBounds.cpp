#include "BoxSphereBounds.h"

#include "AxisAlignedBox.h"
#include "ICollider.h"

Vector& BoxSphereBounds::Origin()
{
	return m_origin;
}

const Vector& BoxSphereBounds::Origin() const
{
	return m_origin;
}

Vector& BoxSphereBounds::HalfSize()
{
	return m_halfSize;
}

const Vector& BoxSphereBounds::HalfSize() const
{
	return m_halfSize;
}

float& BoxSphereBounds::Radius()
{
	return m_radius;
}

float BoxSphereBounds::Radius() const
{
	return m_radius;
}

float BoxSphereBounds::Volume() const
{
	// ( ( 4 / 3 ) * pi * r^3 )
	float sphereVolume = 1.33333f * DirectX::XM_PI * m_radius * m_radius * m_radius;

	// l * w * h
	float boxVolume = 2.f * m_halfSize.x * m_halfSize.y * m_halfSize.z;

	return std::min( sphereVolume, boxVolume );
}

BoxSphereBounds BoxSphereBounds::TransformBy( const Matrix& m ) const
{
	Vector newOrigin = m.TransformPosition( m_origin );

	Vector halfSizeX( m_halfSize.x, m_halfSize.x, m_halfSize.x );
	Vector halfSizeY( m_halfSize.y, m_halfSize.y, m_halfSize.y );
	Vector halfSizeZ( m_halfSize.z, m_halfSize.z, m_halfSize.z );

	Vector m0( m._11, m._12, m._13 );
	Vector m1( m._21, m._22, m._23 );
	Vector m2( m._31, m._32, m._33 );

	Vector newHalfSize = ( halfSizeX * m0 ).GetAbs() + ( halfSizeY * m1 ).GetAbs() + ( halfSizeZ * m2 ).GetAbs();

	XMVector xmMaxRadius = m0 * m0;
	xmMaxRadius = m1 * m1 + xmMaxRadius;
	Vector maxRadius = m2 * m2 + xmMaxRadius;

	float newRadius = std::sqrt( std::max( { maxRadius.x, maxRadius.y, maxRadius.z } ) ) * m_radius;

	newRadius = std::min( newHalfSize.Length(), newRadius );;
	return BoxSphereBounds( newOrigin, newHalfSize, newRadius );
}

CollisionResult BoxSphereBounds::Overlapped( const BoxSphereBounds& other ) const
{
	const Point& lhsPos = Origin();
	const Point& rhsPos = other.Origin();

	Vector midline = lhsPos - rhsPos;
	float size = midline.Length();

	if ( size <= 0.f || size >= Radius() + other.Radius() )
	{
		return CollisionResult::Outside;
	}

	return CollisionResult::Intersection;
}

BoxSphereBounds::BoxSphereBounds( const Vector* points, int32 numPoints )
{
	AxisAlignedBox box( points, numPoints );

	m_halfSize = box.Size() * 0.5f;
	m_origin = box.GetMin() + m_halfSize;

	m_radius = m_halfSize.Length();
}

BoxSphereBounds& BoxSphereBounds::operator+=( const BoxSphereBounds& other )
{
	*this = *this + other;
	return *this;
}

BoxSphereBounds operator+( const BoxSphereBounds& lhs, const BoxSphereBounds& rhs )
{
	Vector points[] = {
		lhs.m_origin - lhs.m_halfSize,
		lhs.m_origin + lhs.m_halfSize,
		rhs.m_origin - rhs.m_halfSize,
		rhs.m_origin + rhs.m_halfSize,
	};

	BoxSphereBounds bounds( points, 4 );

	bounds.m_radius = std::min( bounds.m_radius, std::max( ( lhs.m_origin - bounds.m_origin ).Length() + lhs.m_radius, ( rhs.m_origin - bounds.m_origin ).Length() + rhs.m_radius ) );

	return bounds;
}

float CalcGrowth( const BoxSphereBounds& bounds, const BoxSphereBounds& other )
{
	BoxSphereBounds merged = bounds + other;
	return merged.Volume() - bounds.Volume();
}