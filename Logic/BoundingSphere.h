#pragma once
#include "IRigidBody.h"

#include "../shared/Math/CXMFloat.h"

class BoundingSphere : public IRigidBody
{
public:
	virtual void CreateRigideBody( std::shared_ptr<IMesh> pMesh ) override;
	virtual void Update( const CXMFLOAT4X4& matrix, std::shared_ptr<IRigidBody> original ) override;
	virtual float Intersect( const CRay* ray ) const override;

private:
	CXMFLOAT3 m_origin;
	float m_radiusSqr = 0.f;
};

