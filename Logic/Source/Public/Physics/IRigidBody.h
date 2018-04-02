#pragma once

#include "Math/CXMFloat.h"

#include <memory>

class IMesh;
class CRay;

class IRigidBody
{
public:
	virtual void CreateRigideBody( const IMesh& mesh ) = 0;
	virtual void Update( const CXMFLOAT4X4& matrix, IRigidBody* original ) = 0;
	virtual float Intersect( const CRay* ray ) const = 0;

	virtual ~IRigidBody( ) = default;
};