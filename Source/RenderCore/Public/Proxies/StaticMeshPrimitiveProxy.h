#pragma once

#include "LibraryTool/Common.h"
#include "PrimitiveProxy.h"

namespace logic
{
	class StaticMeshComponent;
}

namespace rendercore
{
	class Material;
	class RenderOption;
	class StaticMesh;
	class StaticMeshRenderData;

	class StaticMeshPrimitiveProxy final : public PrimitiveProxy
	{
	public:
		RENDERCORE_DLL explicit StaticMeshPrimitiveProxy( const logic::StaticMeshComponent& component );

		virtual void CreateRenderData() override;
		virtual void PrepareSubMeshs() override;
		virtual void TakeSnapshot( RenderFrameArray<DrawSnapshot>& outSnapshotStorage, RenderFrameArray<VisibleDrawSnapshot>& outVisibleSnapshot ) const override;
		virtual std::optional<DrawSnapshot> TakeSnapshot( uint32 lod, uint32 sectionIndex ) const override;
		virtual std::optional<MeshDrawInfo> GatherMeshDrawInfo( uint32 lod, uint32 sectionIndex ) const override;

		virtual void AddToRaytracingScene( RaytracingScene& raytracingScene ) override;

	private:
		std::shared_ptr<const StaticMesh> m_pStaticMesh = nullptr;
		StaticMeshRenderData* m_pRenderData = nullptr;

		std::shared_ptr<const RenderOption> m_pRenderOption = nullptr;

		std::vector<std::shared_ptr<Material>> m_materials;
	};
}
