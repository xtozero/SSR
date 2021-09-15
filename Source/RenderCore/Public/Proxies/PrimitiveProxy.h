#pragma once

#include "Math/CXMFloat.h"
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
	void SetTransform( const CXMFLOAT4X4& worldTransform );
	const CXMFLOAT4X4& GetTransform( ) const;

	virtual void CreateRenderData( ) = 0;
	virtual void PrepareSubMeshs( ) = 0;
	virtual void GetSubMeshElement( uint32 lod, uint32 sectionIndex, PrimitiveSubMesh& subMesh ) = 0;
	virtual void TakeSnapshot( std::deque<DrawSnapshot>& snapshotStorage, SceneViewConstantBuffer& viewConstant, std::vector<VisibleDrawSnapshot>& drawList ) const = 0;
	virtual std::optional<DrawSnapshot> TakeSnapshot( uint32 lod, uint32 sectionIndex, SceneViewConstantBuffer& viewConstant ) const = 0;

	virtual ~PrimitiveProxy( ) = default;

	uint32 PrimitiveId( ) const;

protected:
	friend Scene;

	PrimitiveSceneInfo* m_primitiveSceneInfo = nullptr;
	CXMFLOAT4X4 m_worldTransform;
};