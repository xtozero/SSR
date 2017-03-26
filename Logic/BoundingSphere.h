#pragma once
#include "IRigidBody.h"

#include "../shared/CDirectXMath.h"

class BoundingSphere : public IRigidBody
{
public:
	virtual void CreateRigideBody( std::shared_ptr<IMesh> pMesh ) override;
	virtual void Update( const D3DXMATRIX& matrix, std::shared_ptr<IRigidBody> original ) override;
	virtual float Intersect( const CRay* ray ) const override;

private:
	CXMFLOAT3 m_origin = { 0.f, 0.f, 0.f };
	float m_radiusSqr = 0.f;
};

