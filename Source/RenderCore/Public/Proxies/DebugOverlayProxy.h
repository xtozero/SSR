#pragma once

#include "common.h"
#include "Components/DebugOverlayComponent.h"
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
		RENDERCORE_DLL DebugOverlayProxy( const logic::DebugOverlayComponent& component );

		virtual void CreateRenderData() override {}
		virtual void PrepareSubMeshs() override {}
		virtual void TakeSnapshot( [[maybe_unused]] std::deque<DrawSnapshot>& snapshotStorage, [[maybe_unused]]  RenderThreadFrameData<VisibleDrawSnapshot>& drawList ) const {}
		virtual std::optional<DrawSnapshot> TakeSnapshot( [[maybe_unused]] uint32 lod, [[maybe_unused]] uint32 sectionIndex ) const
		{
			return {};
		}
		virtual MeshDrawInfo GatherMeshDrawInfo( [[maybe_unused]] uint32 lod, [[maybe_unused]] uint32 sectionIndex ) const override
		{
			return {};
		}
		virtual void GatherDynamicMeshDrawInfo( RenderViewInfo& viewInfo ) const override;

	private:
		std::vector<logic::DebugLine> m_debugLine;
		std::vector<logic::DebugTriangle> m_debugTriangle;
	};
}