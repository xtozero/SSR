#pragma once

#include "IRigidBody.h"

#include "../shared/CDirectXMath.h"

class CAaboundingbox : public IRigidBody
{
public:
	virtual void CreateRigideBody( std::shared_ptr<IMesh> pMesh ) override;
	virtual void Update( const CXMFLOAT4X4& matrix, std::shared_ptr<IRigidBody> original ) override;
	virtual float Intersect( const CRay* ray ) const override;

private:
	CXMFLOAT3 m_max;
	CXMFLOAT3 m_min;

public:
	CAaboundingbox( );
	~CAaboundingbox( );
};

