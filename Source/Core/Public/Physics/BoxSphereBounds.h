#pragma once

#include "Archive.h"
#include "ArchiveUtility.h"
#include "Aaboundingbox.h"
#include "BoundingSphere.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

class BoxSphereBounds
{
public:
	Vector& Origin( );
	const Vector& Origin( ) const;

	Vector& HalfSize( );
	const Vector& HalfSize( ) const;

	float& Radius( );
	float Radius( ) const;

	BoxSphereBounds TransformBy( const Matrix& m ) const;

	BoxSphereBounds( ) = default;
	BoxSphereBounds( const Vector& origin, const Vector& halfSize, float radius ) : m_origin( origin ), m_halfSize( halfSize ), m_radius( radius ) {}
	BoxSphereBounds( const Vector* points, int32 numPoints )
	{
		CAaboundingbox box( points, numPoints );

		m_halfSize = box.Size( ) * 0.5;
		m_origin = box.GetMin( ) + m_halfSize;

		m_radius = m_halfSize.Length( );
	}

	friend Archive& operator<<( Archive& ar, BoxSphereBounds& bounds )
	{
		ar << bounds.m_origin << bounds.m_halfSize << bounds.m_radius;
		return ar;
	}

private:
	Vector m_origin;
	Vector m_halfSize;
	float m_radius = 0.f;
};
