#pragma once

#include <memory>

#include "../shared/Math/CXMFloat.h"

class IMesh;
class CRay;

class IRigidBody
{
public:
	virtual void CreateRigideBody( std::shared_ptr<IMesh> pMesh ) = 0;
	virtual void Update( const CXMFLOAT4X4& matrix, std::shared_ptr<IRigidBody> original ) = 0;
	virtual float Intersect( const CRay* ray ) const = 0;

	virtual ~IRigidBody( ) = default;
};