#pragma once

#include "common.h"
#include "PrimitiveProxy.h"

class RenderOption;
class StaticMesh;
class StaticMeshComponent;
class StaticMeshRenderData;

class StaticMeshPrimitiveProxy : public PrimitiveProxy
{
public:
	RENDERCORE_DLL StaticMeshPrimitiveProxy( const StaticMeshComponent& component );

	virtual void CreateRenderData() override;
	virtual void PrepareSubMeshs() override;
	virtual void TakeSnapshot( std::deque<DrawSnapshot>& snapshotStorage, rendercore::VectorSingleFrame<VisibleDrawSnapshot>& drawList ) const override;
	virtual std::optional<DrawSnapshot> TakeSnapshot( uint32 lod, uint32 sectionIndex ) const override;
	virtual MeshDrawInfo GatherMeshDrawInfo( uint32 lod, uint32 sectionIndex ) const override;

private:

	std::shared_ptr<const StaticMesh> m_pStaticMesh = nullptr;
	StaticMeshRenderData* m_pRenderData = nullptr;

	std::shared_ptr<const RenderOption> m_pRenderOption = nullptr;
};