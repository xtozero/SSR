#pragma once

#include "ICollider.h"
#include "Math/CXMFloat.h"

class CAaboundingbox;

class COrientedBoundingBox : public ICollider
{
public:
	virtual void CalcMeshBounds( const MeshData& mesh ) override;
	virtual void Update( const CXMFLOAT3& scaling, const CXMFLOAT4& rotation, const CXMFLOAT3& translation, ICollider* original ) override;
	virtual void CalcSubMeshBounds( std::vector<std::unique_ptr<ICollider>>& subColliders ) override;
	virtual float Intersect( const CRay& ray ) const override;
	virtual int Intersect( const CFrustum& frustum ) const override;
	virtual void DrawDebugOverlay( CDebugOverlayManager& debugOverlay, unsigned int color, float duration ) const override;

	CXMFLOAT3 GetAxisVector( int i ) const;
	CXMFLOAT3 GetHalfSize( ) const { return m_halfSize; }
	const CXMFLOAT4X4& GetTransform( ) const { return m_matTransform; }

	COrientedBoundingBox( ) = default;
	explicit COrientedBoundingBox( const CAaboundingbox& box );

private:
	CXMFLOAT3 m_halfSize = { 0.f, 0.f, 0.f };
	CXMFLOAT4X4 m_matTransform;
	CXMFLOAT4X4 m_matInvTransform;
};

float CalcPenetrationOnAxis( const COrientedBoundingBox& lhs, const COrientedBoundingBox& rhs, const CXMFLOAT3& axis, const CXMFLOAT3& toCentre );
bool TryAxis( const COrientedBoundingBox& lhs, const COrientedBoundingBox& rhs, const CXMFLOAT3& axis, const CXMFLOAT3& toCentre, unsigned int index, float& smallestPenetration, unsigned int& smallestCase );