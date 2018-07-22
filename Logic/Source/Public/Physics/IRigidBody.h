#pragma once

#include "Math/CXMFloat.h"

#include <memory>
#include <vector>

class CFrustum;
class CRay;
class IMesh;

namespace COLLISION
{
	enum TYPE
	{
		OUTSIDE = 0,
		INSIDE,
		INTERSECTION
	};
}

class IRigidBody
{
public:
	virtual void CreateRigideBody( const IMesh& mesh ) = 0;
	virtual void Update( const CXMFLOAT4X4& matrix, IRigidBody* original ) = 0;
	virtual void CalcSubRigidBody( std::vector<std::unique_ptr<IRigidBody>>& subRigidBody ) = 0;
	virtual float Intersect( const CRay* ray ) const = 0;
	virtual int Intersect( const CFrustum& frustum ) const = 0;

	virtual ~IRigidBody( ) = default;
};