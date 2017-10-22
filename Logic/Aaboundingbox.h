#pragma once

#include "IRigidBody.h"

#include "../shared/Math/CXMFloat.h"

class CAaboundingbox : public IRigidBody
{
public:
	virtual void CreateRigideBody( const IMesh& mesh ) override;
	virtual void Update( const CXMFLOAT4X4& matrix, IRigidBody* original ) override;
	virtual float Intersect( const CRay* ray ) const override;

private:
	CXMFLOAT3 m_max;
	CXMFLOAT3 m_min;

public:
	CAaboundingbox( );
	~CAaboundingbox( );
};

