#include "PerformanceProfilePanel.h"

#include "CpuProfiler.h"
#include "GpuProfiler/GpuProfiler.h"
#include "IEditor.h"
#include "imgui.h"
#include "LibraryTool/InterfaceFactories.h"
#include "Multithread/TaskScheduler.h"
#include "PanelFactory.h"
#include "PanelSharedContext.h"

using ::engine::CpuProfileData;
using ::engine::ICpuProfiler;
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
		ImGui::TextColored( ImVec4( 0.0f, 1.0f, 0.0f, 1.0f ), "[GPU]" );

		const std::vector<GpuProfileData*>& gpuProfileDatas = gpuProfiler->GetProfileDatas();

		for ( const GpuProfileData* gpuProfileData : gpuProfileDatas )
		{
			if ( gpuProfileData->m_parent == nullptr )
			{
				DrawGpuProfileRecursive( gpuProfileData );
			}
		}
	}

	void DrawCpuProfileRecursive( const CpuProfileData* cpuProfileData )
	{
		constexpr ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		const char* label = cpuProfileData->m_label.CStr();
		ImGuiTreeNodeFlags nodeFlags = baseFlags;
		if ( cpuProfileData->m_child == nullptr )
		{
			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		if ( ImGui::TreeNodeEx( label, nodeFlags, "%s - %fms", label, (float)cpuProfileData->m_averageMS ) )
		{
			if ( const CpuProfileData* child = cpuProfileData->m_child )
			{
				DrawCpuProfileRecursive( child );
				ImGui::TreePop();
			}
		}

		if ( const CpuProfileData* sibling = cpuProfileData->m_sibling )
		{
			DrawCpuProfileRecursive( sibling );
		}
	}

	void DrawCpuProfile( const ICpuProfiler* cpuProfiler, std::thread::id threadId )
	{
		static std::vector<const CpuProfileData*> cpuProfileDatas;

		cpuProfileDatas.clear();
		cpuProfiler->GetProfileDatas( threadId, cpuProfileDatas );

		for ( const CpuProfileData* cpuProfileData : cpuProfileDatas )
		{
			DrawCpuProfileRecursive( cpuProfileData );
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

		ImGui::SetNextWindowSizeConstraints( ImVec2( -1, 200 ), ImVec2( -1, 400 ) );
		ImGui::Begin( "Performance Profile", &shouldDraw, ImGuiWindowFlags_AlwaysAutoResize );
		{
			static const auto* gpuProfiler = GetInterface<IGpuProfiler>();
			DrawGpuProfile( gpuProfiler );

			ImGui::TextColored( ImVec4( 0.0f, 1.0f, 0.0f, 1.0f ), "[RenderThread]" );

			static const auto* cpuProfiler = GetInterface<ICpuProfiler>();
			static const auto* taskScheduler = GetInterface<ITaskScheduler>();

			std::thread::id renderThreadId = taskScheduler->GetThreadId( ThreadType::RenderThread );
			DrawCpuProfile( cpuProfiler, renderThreadId );
		}
		ImGui::End();

		sharedCtx.OpenProfiler( shouldDraw );
	}
}