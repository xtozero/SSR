#pragma once

#include "Archive.h"
#include "ArchiveUtility.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

enum class CollisionResult : uint8;

class BoxSphereBounds
{
public:
	Vector& Origin();
	const Vector& Origin() const;

	Vector& HalfSize();
	const Vector& HalfSize() const;

	float& Radius();
	float Radius() const;

	float Volume() const;

	BoxSphereBounds TransformBy( const Matrix& m ) const;

	CollisionResult Overlapped( const BoxSphereBounds& other ) const;

	BoxSphereBounds() = default;
	BoxSphereBounds( const Vector& origin, const Vector& halfSize, float radius ) : m_origin( origin ), m_halfSize( halfSize ), m_radius( radius ) {}
	BoxSphereBounds( const Vector* points, int32 numPoints );

	BoxSphereBounds& operator+=( const BoxSphereBounds& other );

	friend BoxSphereBounds operator+( const BoxSphereBounds& lhs, const BoxSphereBounds& rhs );

	friend Archive& operator<<( Archive& ar, BoxSphereBounds& bounds )
	{
		ar << bounds.m_origin << bounds.m_halfSize << bounds.m_radius;

		return ar;
	}

	friend float CalcGrowth( const BoxSphereBounds& bounds, const BoxSphereBounds& other );

private:
	Vector m_origin = Vector::ZeroVector;
	Vector m_halfSize = Vector::ZeroVector;
	float m_radius = 0.f;
};
