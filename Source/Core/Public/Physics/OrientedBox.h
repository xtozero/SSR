#pragma once

#include "ICollider.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

class AxisAlignedBox;

class OrientedBox : public ICollider
{
public:
	virtual void Update( const Vector& scaling, const Quaternion& rotation, const Vector& translation, ICollider* original ) override;
	virtual float Intersect( const CRay& ray ) const override;
	virtual uint32 Intersect( const Frustum& frustum ) const override;
	virtual BoxSphereBounds Bounds() const override;
	virtual Collider GetType() const override;

	Vector GetAxisVector( uint32 i ) const;
	Vector GetHalfSize() const { return m_halfSize; }
	const Matrix& GetTransform() const { return m_matTransform; }

	OrientedBox() = default;
	explicit OrientedBox( const AxisAlignedBox& box );
	OrientedBox( const Vector& halfSize, const Matrix& transform );

private:
	Vector m_halfSize;
	Matrix m_matTransform;
	Matrix m_matInvTransform;
};

bool TryAxis( const OrientedBox& lhs, const OrientedBox& rhs, const Vector& axis, const Vector& toCentre, uint32 index, float& smallestPenetration, uint32& smallestCase );