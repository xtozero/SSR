#pragma once

#include "ICollider.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

class CAaboundingbox;

class COrientedBoundingBox : public ICollider
{
public:
	virtual void CalcMeshBounds( const MeshData& mesh ) override;
	virtual void Update( const Vector& scaling, const Quaternion& rotation, const Vector& translation, ICollider* original ) override;
	virtual void CalcSubMeshBounds( std::vector<std::unique_ptr<ICollider>>& subColliders ) override;
	virtual float Intersect( const CRay& ray ) const override;
	virtual uint32 Intersect( const Frustum& frustum ) const override;

	Vector GetAxisVector( uint32 i ) const;
	Vector GetHalfSize( ) const { return m_halfSize; }
	const Matrix& GetTransform( ) const { return m_matTransform; }

	COrientedBoundingBox( ) = default;
	explicit COrientedBoundingBox( const CAaboundingbox& box );

private:
	Vector m_halfSize;
	Matrix m_matTransform;
	Matrix m_matInvTransform;
};

float CalcPenetrationOnAxis( const COrientedBoundingBox& lhs, const COrientedBoundingBox& rhs, const Vector& axis, const Vector& toCentre );
bool TryAxis( const COrientedBoundingBox& lhs, const COrientedBoundingBox& rhs, const Vector& axis, const Vector& toCentre, uint32 index, float& smallestPenetration, uint32& smallestCase );