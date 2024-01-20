#include "PerformanceProfilePanel.h"

#include "GpuProfiler/GPUProfiler.h"
#include "IEditor.h"
#include "imgui.h"
#include "LibraryTool/InterfaceFactories.h"
#include "PanelFactory.h"
#include "PanelSharedContext.h"

using ::rendercore::GpuProfileData;
using ::rendercore::IGpuProfiler;

namespace
{
	void DrawGpuProfileRecursive( const GpuProfileData* gpuProfileData )
	{
		constexpr ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		const char* label = gpuProfileData->m_label.CStr();
		ImGuiTreeNodeFlags nodeFlags = baseFlags;
		if ( gpuProfileData->m_child == nullptr )
		{
			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		if ( ImGui::TreeNodeEx( label, nodeFlags, "%s - %fms", label, (float)gpuProfileData->m_averageMS) )
		{
			if ( const GpuProfileData* child = gpuProfileData->m_child )
			{
				DrawGpuProfileRecursive( child );
				ImGui::TreePop();
			}
		}

		if ( const GpuProfileData* sibling = gpuProfileData->m_sibling )
		{
			DrawGpuProfileRecursive( sibling );
		}
	}

	void DrawGpuProfile( const IGpuProfiler* gpuProfiler )
	{
		const std::vector<GpuProfileData*>& gpuProfileDatas = gpuProfiler->GetProfileDatas();

		for ( const GpuProfileData* gpuProfileData : gpuProfileDatas )
		{
			if ( gpuProfileData->m_parent == nullptr )
			{
				DrawGpuProfileRecursive( gpuProfileData );
			}
		}
	}
}

namespace editor
{
	REGISTER_PANEL( PerformanceProfilePanel );
	void PerformanceProfilePanel::Draw( IEditor& editor )
	{
		PanelSharedContext& sharedCtx = editor.GetPanelSharedCtx();
		bool shouldDraw = sharedCtx.ShouldDrawProfiler();
		if ( shouldDraw == false )
		{
			return;
		}

		ImGui::Begin( "Performance Profile", &shouldDraw );
		{
			static const auto* gpuProfiler = GetInterface<IGpuProfiler>();
			DrawGpuProfile( gpuProfiler );
		}
		ImGui::End();

		sharedCtx.OpenProfiler( shouldDraw );
	}
}