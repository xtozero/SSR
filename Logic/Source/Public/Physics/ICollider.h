#pragma once

#include "Math/CXMFloat.h"

#include <memory>
#include <vector>

class CDebugOverlayManager;
class CFrustum;
class CRay;
class IMesh;

namespace COLLISION
{
	enum TYPE
	{
		OUTSIDE = 0,
		INSIDE,
		INTERSECTION,
	};
}

class ICollider
{
public:
	virtual void CalcMeshBounds( const IMesh& mesh ) = 0;
	virtual void Update( const CXMFLOAT3& scaling, const CXMFLOAT3& rotation, const CXMFLOAT3& translation, ICollider* original ) = 0;
	virtual void CalcSubMeshBounds( std::vector<std::unique_ptr<ICollider>>& subColliders ) = 0;
	virtual float Intersect( const CRay* ray ) const = 0;
	virtual int Intersect( const CFrustum& frustum ) const = 0;
	virtual void DrawDebugOverlay( CDebugOverlayManager& debugOverlay ) const = 0;

	virtual ~ICollider( ) = default;
};