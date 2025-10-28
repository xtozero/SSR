#pragma once

#include "Components/DebugOverlayComponent.h"
#include "LibraryTool/Common.h"
#include "PrimitiveProxy.h"

namespace logic
{
	class DebugOverlayComponent;
}

namespace rendercore
{
	class DebugOverlayProxy final : public PrimitiveProxy
	{
	public:
		RENDERCORE_DLL explicit DebugOverlayProxy( const logic::DebugOverlayComponent& component );

		virtual void CreateRenderData() override {}
		virtual void PrepareSubMeshs() override {}
		virtual void TakeSnapshot( [[maybe_unused]] RenderFrameArray<DrawSnapshot>& outSnapshotStorage, [[maybe_unused]]  RenderFrameArray<VisibleDrawSnapshot>& outVisibleSnapshot ) const override {}
		virtual std::optional<DrawSnapshot> TakeSnapshot( [[maybe_unused]] uint32 lod, [[maybe_unused]] uint32 sectionIndex ) const override
		{
			return {};
		}
		virtual std::optional<MeshDrawInfo> GatherMeshDrawInfo( [[maybe_unused]] uint32 lod, [[maybe_unused]] uint32 sectionIndex ) const override
		{
			return {};
		}
		virtual void GatherDynamicMeshDrawInfo( RenderViewInfo& viewInfo ) const override;

	private:
		std::vector<logic::DebugLine> m_debugLine;
		std::vector<logic::DebugTriangle> m_debugTriangle;
	};
}