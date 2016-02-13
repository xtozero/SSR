#pragma once

#include "IRigidBody.h"

class CAaboundingbox : public IRigidBody
{
public:
	virtual void CreateRigideBody( std::shared_ptr<IMesh> pMesh ) override;
	virtual void Update( const D3DXMATRIX& matrix, std::shared_ptr<IRigidBody> original ) override;
	virtual float Intersect( const CRay* ray ) const override;

private:
	D3DXVECTOR3 m_max;
	D3DXVECTOR3 m_min;

public:
	CAaboundingbox( );
	~CAaboundingbox( );
};

