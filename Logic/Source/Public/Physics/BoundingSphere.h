#pragma once

#include "IRigidBody.h"
#include "Math/CXMFloat.h"

#include <vector>

class CAaboundingbox;

class BoundingSphere : public IRigidBody
{
public:
	virtual void CreateRigideBody( const IMesh& mesh ) override;
	virtual void Update( const CXMFLOAT4X4& matrix, IRigidBody* original ) override;
	virtual void CalcSubRigidBody( std::vector<std::unique_ptr<IRigidBody>>& /*subRigidBody*/ ) override { assert( false && "Not Implemented" ); }
	virtual float Intersect( const CRay* ray ) const override;
	virtual int Intersect( const CFrustum& frustum ) const override;
	int Intersect( const BoundingSphere& sphere ) const;
	float CalcGrowth( const BoundingSphere& sphere ) const;

	bool Intersect( const CFrustum& frustum, const CXMFLOAT3& sweepDir );
	const CXMFLOAT3& GetCenter( ) const { return m_origin; }
	float GetRadius( ) const { return m_radius; }
	float GetSize( ) const { return 1.33333f * DirectX::XM_PI * m_radius * m_radius * m_radius; /* ( ( 4 / 3 ) * pi * r^3 ) */ }

	BoundingSphere( ) = default;
	explicit BoundingSphere( const CAaboundingbox& box );
	explicit BoundingSphere( const std::vector<CXMFLOAT3>& points );
	BoundingSphere( const CXMFLOAT3& center, float radius ) : m_origin( center ), m_radius( radius ) {}
	BoundingSphere( const BoundingSphere& one, const BoundingSphere& two );

private:
	CXMFLOAT3 m_origin;
	float m_radius = 0.f;
};
