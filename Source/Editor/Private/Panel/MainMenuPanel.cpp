#include "MainMenuPanel.h"

#include "imgui.h"
#include "PanelFactory.h"

namespace editor
{
	REGISTER_PANEL( MainMenuPanel );
	void MainMenuPanel::Draw( [[maybe_unused]] IEditor& editor )
	{
		if ( ImGui::BeginMainMenuBar() )
		{
			ImGui::EndMainMenuBar();
		}
	}

	void MainMenuPanel::HandleUserInput( [[maybe_unused]] const engine::UserInput& input )
	{
	}
}
