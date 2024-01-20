#include "MainMenuPanel.h"

#include "IEditor.h"
#include "imgui.h"
#include "PanelFactory.h"
#include "PanelSharedContext.h"

namespace editor
{
	REGISTER_PANEL( MainMenuPanel );
	void MainMenuPanel::Draw( IEditor& editor )
	{
		PanelSharedContext& sharedCtx = editor.GetPanelSharedCtx();

		if ( ImGui::BeginMainMenuBar() )
		{
			if ( ImGui::BeginMenu( "Window" ) )
			{
				// Profiler
				{
					bool showProfiler = sharedCtx.ShouldDrawProfiler();
					ImGui::MenuItem( "Profiler", nullptr, &showProfiler );
					sharedCtx.OpenProfiler( showProfiler );
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
