#pragma once

#include "Math/CXMFloat.h"
#include "Physics/BoxSphereBounds.h"
#include "SizedTypes.h"

#include <deque>
#include <optional>
#include <vector>

class DrawSnapshot;
class PrimitiveSceneInfo;
class PrimitiveSubMesh;
class Scene;
class ScenePrimitiveBuffer;
class SceneViewConstantBuffer;
class VisibleDrawSnapshot;

class PrimitiveProxy
{
public:
	virtual void CreateRenderData( ) = 0;
	virtual void PrepareSubMeshs( ) = 0;
	virtual void GetSubMeshElement( uint32 lod, uint32 sectionIndex, PrimitiveSubMesh& subMesh ) = 0;
	virtual void TakeSnapshot( std::deque<DrawSnapshot>& snapshotStorage, SceneViewConstantBuffer& viewConstant, std::vector<VisibleDrawSnapshot>& drawList ) const = 0;
	virtual std::optional<DrawSnapshot> TakeSnapshot( uint32 lod, uint32 sectionIndex, SceneViewConstantBuffer& viewConstant ) const = 0;

	CXMFLOAT4X4& WorldTransform( );
	const CXMFLOAT4X4& WorldTransform( ) const;

	BoxSphereBounds& Bounds( );
	const BoxSphereBounds& Bounds( ) const;
	
	BoxSphereBounds& LocalBounds( );
	const BoxSphereBounds& LocalBounds( ) const;
	uint32 PrimitiveId( ) const;

	virtual ~PrimitiveProxy( ) = default;

protected:
	friend Scene;

	PrimitiveSceneInfo* m_primitiveSceneInfo = nullptr;
	CXMFLOAT4X4 m_worldTransform;
	BoxSphereBounds m_bounds;
	BoxSphereBounds m_localBounds;
};