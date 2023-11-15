#pragma once

#include "common.h"
#include "PrimitiveProxy.h"

namespace logic
{
	class StaticMeshComponent;
}

namespace rendercore
{
	class RenderOption;
	class StaticMesh;
	class StaticMeshRenderData;

	class StaticMeshPrimitiveProxy final : public PrimitiveProxy
	{
	public:
		RENDERCORE_DLL StaticMeshPrimitiveProxy( const logic::StaticMeshComponent& component );

		virtual void CreateRenderData() override;
		virtual void PrepareSubMeshs() override;
		virtual void TakeSnapshot( std::deque<DrawSnapshot>& snapshotStorage, RenderThreadFrameData<VisibleDrawSnapshot>& drawList ) const override;
		virtual std::optional<DrawSnapshot> TakeSnapshot( uint32 lod, uint32 sectionIndex ) const override;
		virtual MeshDrawInfo GatherMeshDrawInfo( uint32 lod, uint32 sectionIndex ) const override;

	private:

		std::shared_ptr<const StaticMesh> m_pStaticMesh = nullptr;
		StaticMeshRenderData* m_pRenderData = nullptr;

		std::shared_ptr<const RenderOption> m_pRenderOption = nullptr;
	};
}
