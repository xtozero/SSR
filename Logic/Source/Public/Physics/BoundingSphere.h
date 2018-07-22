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
	virtual void CalcSubRigidBody( std::vector<std::unique_ptr<IRigidBody>>& subRigidBody ) override { assert( false && "Not Implemented" ); }
	virtual float Intersect( const CRay* ray ) const override;
	virtual int Intersect( const CFrustum& frustum ) const override;

	bool Intersect( const CFrustum& frustum, const CXMFLOAT3& sweepDir );
	const CXMFLOAT3& GetCenter( ) const { return m_origin; }
	float GetRadius( ) const { return sqrtf( m_radiusSqr ); }

	BoundingSphere( ) = default;
	explicit BoundingSphere( const CAaboundingbox& box );
	explicit BoundingSphere( const std::vector<CXMFLOAT3>& points );
	BoundingSphere( const CXMFLOAT3& center, float radiusSqr ) : m_origin( center ), m_radiusSqr( radiusSqr ) {}

private:
	CXMFLOAT3 m_origin;
	float m_radiusSqr = 0.f;
};

