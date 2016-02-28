#pragma once

#include <d3dX9math.h>
#include <memory>

class IMesh;
class CRay;

class IRigidBody
{
public:
	virtual void CreateRigideBody( std::shared_ptr<IMesh> pMesh ) = 0;
	virtual void Update( const D3DXMATRIX& matrix, std::shared_ptr<IRigidBody> original ) = 0;
	virtual float Intersect( const CRay* ray ) const = 0;

	virtual ~IRigidBody( ) = default;
};