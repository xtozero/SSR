#pragma once

#include "ICollider.h"
#include "Math/Vector.h"
#include "SizedTypes.h"

#include <vector>

class CAaboundingbox;
class COrientedBoundingBox;

class BoundingSphere : public ICollider
{
public:
	virtual void CalcMeshBounds( const MeshData& mesh ) override;
	virtual void Update( const Vector& scaling, const Quaternion& rotation, const Vector& translation, ICollider* original ) override;
	virtual void CalcSubMeshBounds( std::vector<std::unique_ptr<ICollider>>& /*subColliders*/ ) override { assert( false && "Not Implemented" ); }
	virtual float Intersect( const CRay& ray ) const override;
	virtual uint32 Intersect( const Frustum& frustum ) const override;
	uint32 Intersect( const BoundingSphere& sphere ) const;
	float CalcGrowth( const BoundingSphere& sphere ) const;

	bool Intersect( const Frustum& frustum, const Vector& sweepDir );
	const Vector& GetCenter( ) const { return m_origin; }
	float GetRadius( ) const { return m_radius; }
	float GetSize( ) const { return 1.33333f * DirectX::XM_PI * m_radius * m_radius * m_radius; /* ( ( 4 / 3 ) * pi * r^3 ) */ }

	BoundingSphere( ) = default;
	explicit BoundingSphere( const CAaboundingbox& box );
	explicit BoundingSphere( const COrientedBoundingBox& box );
	explicit BoundingSphere( const std::vector<Vector>& points );
	BoundingSphere( const Vector& center, float radius ) : m_origin( center ), m_radius( radius ) {}
	BoundingSphere( const BoundingSphere& one, const BoundingSphere& two );

private:
	Vector m_origin;
	float m_radius = 0.f;
};
