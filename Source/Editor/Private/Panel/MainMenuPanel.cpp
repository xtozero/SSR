#include "MainMenuPanel.h"

#include "ExternalTools/AssetBuilderLauncher.h"
#include "IEditor.h"
#include "imgui.h"
#include "Renderer/IRenderCore.h"
#include "PanelFactory.h"
#include "PanelSharedContext.h"

namespace editor
{
	REGISTER_PANEL( MainMenuPanel );
	void MainMenuPanel::Draw()
	{
		IEditor& editor = GetEditor();
		PanelSharedContext& sharedCtx = editor.GetPanelSharedCtx();

		if ( ImGui::BeginMainMenuBar() )
		{
			if ( ImGui::BeginMenu( "Window" ) )
			{
				// Profiler
				bool showProfiler = sharedCtx.ShouldDrawProfiler();
				if ( ImGui::MenuItem( "Profiler", nullptr, &showProfiler ) )
				{
					sharedCtx.OpenProfiler( showProfiler );
				}
				
				// Pipeline Statistics
				bool showPipelineStatistics = sharedCtx.ShouldDrawPipelineStatistics();
				if ( ImGui::MenuItem( "Pipeline Statistics", nullptr, &showPipelineStatistics ) )
				{
					sharedCtx.OpenPipelineStatistics( showPipelineStatistics );
				}

				ImGui::EndMenu();
			}

			if ( ImGui::BeginMenu( "Tools" ) )
			{
				// Asset Builder
				if ( ImGui::MenuItem( "Run Asset Builder", nullptr ) )
				{
					engine::RunAssetBuilder();
				}

				// Reload Shaders
				if ( ImGui::MenuItem( "Reload Shaders", nullptr ) )
				{
					GetInterface<rendercore::IRenderCore>()->ReloadShaders();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void MainMenuPanel::HandleUserInput( [[maybe_unused]] const engine::UserInput& input )
	{
	}
}
