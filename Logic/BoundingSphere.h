#pragma once
#include "IRigidBody.h"

class BoundingSphere : public IRigidBody
{
public:
	virtual void CreateRigideBody( std::shared_ptr<IMesh> pMesh ) override;
	virtual void Update( const D3DXMATRIX& matrix, std::shared_ptr<IRigidBody> original ) override;
	virtual float Intersect( const CRay* ray ) const override;

	BoundingSphere( );

private:
	D3DXVECTOR3 m_origin;
	D3DXVECTOR3 m_far;
	float m_radiusSqr;
};

