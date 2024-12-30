#include "PipelineStatisticsPanel.h"

#include "GpuProfiler/GpuProfiler.h"
#include "IEditor.h"
#include "imgui.h"
#include "PanelFactory.h"
#include "PanelSharedContext.h"

using ::rendercore::IGpuProfiler;
using ::rendercore::PipelineStatData;

namespace
{
	const PipelineStatData* SelectedData = nullptr;

	void DrawDataTableRow( const char* name, uint64 value )
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text( name );
		ImGui::TableNextColumn();
		ImGui::Text( "%lld", value );
	}

	void DrawGpuProfileRecursive( const PipelineStatData* pipelineStatData )
	{
		constexpr ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		const char* label = pipelineStatData->m_label.CStr();
		ImGuiTreeNodeFlags nodeFlags = baseFlags;
		if ( pipelineStatData->m_child == nullptr )
		{
			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		bool isSelected = ( pipelineStatData == SelectedData );
		if ( isSelected )
		{
			nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		if ( ImGui::TreeNodeEx( label, nodeFlags, "%s", label ) )
		{
			if ( ImGui::IsItemClicked() )
			{
				SelectedData = pipelineStatData;
			}

			if ( const PipelineStatData* child = pipelineStatData->m_child )
			{
				DrawGpuProfileRecursive( child );
				ImGui::TreePop();
			}
		}

		if ( const PipelineStatData* sibling = pipelineStatData->m_sibling )
		{
			DrawGpuProfileRecursive( sibling );
		}
	}

	void DrawPipelineStatics( const IGpuProfiler* gpuProfiler )
	{
		ImGui::TextColored( ImVec4( 0.0f, 1.0f, 0.0f, 1.0f ), "[Pipeline Statistics]" );

		if ( ImGui::BeginChild( "##tree", ImVec2( 300, 0 ), ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened ) )
		{
			const std::vector<PipelineStatData*>& pipelineStatData = gpuProfiler->GetPipelineStatData();
			for ( const PipelineStatData* statData : pipelineStatData )
			{
				if ( statData->m_parent == nullptr )
				{
					DrawGpuProfileRecursive( statData );
				}
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginGroup();

		agl::PipelineStatisticsData statData = {};
		if ( SelectedData != nullptr )
		{
			statData = SelectedData->GetStatData();
		}

		if ( ImGui::BeginTable( "Pipeline Statistics", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Borders ) )
		{
			ImGui::TableSetupColumn( "Name" );
			ImGui::TableSetupColumn( "Value" );
			ImGui::TableHeadersRow();

			DrawDataTableRow( "IAVertices", statData.m_verticesIA );
			DrawDataTableRow( "IAPrimitives", statData.m_primitivesIA );
			DrawDataTableRow( "VSInvocations", statData.m_invocationsVS );
			DrawDataTableRow( "GSInvocations", statData.m_invocationsGS );
			DrawDataTableRow( "GSPrimitives", statData.m_primitivesGS );
			DrawDataTableRow( "CInvocations", statData.m_invocationsC );
			DrawDataTableRow( "CPrimitives", statData.m_primitivesC );
			DrawDataTableRow( "PSInvocations", statData.m_invocationsPS );
			DrawDataTableRow( "HSInvocations", statData.m_invocationsHS );
			DrawDataTableRow( "DSInvocations", statData.m_invocationsDS );
			DrawDataTableRow( "CSInvocations", statData.m_invocationsCS );
			DrawDataTableRow( "ASInvocations", statData.m_invocationsAS );
			DrawDataTableRow( "MSInvocations", statData.m_invocationsMS );
			DrawDataTableRow( "MSPrimitives", statData.m_primitivesMS );

			ImGui::EndTable();
		}

		ImGui::EndGroup();
	}
}

namespace editor
{
	REGISTER_PANEL( PipelineStatisticsPanel );

	void PipelineStatisticsPanel::Draw()
	{
		IEditor& editor = GetEditor();
		PanelSharedContext& sharedCtx = editor.GetPanelSharedCtx();
		bool shouldDraw = sharedCtx.ShouldDrawPipelineStatistics();
		if ( shouldDraw == false )
		{
			return;
		}

		ImGui::Begin( "Pipeline Statistics", &shouldDraw, ImGuiWindowFlags_AlwaysAutoResize );
		{
			static const auto* gpuProfiler = GetInterface<IGpuProfiler>();
			DrawPipelineStatics( gpuProfiler );
		}
		ImGui::End();

		sharedCtx.OpenPipelineStatistics( shouldDraw );
	}
}