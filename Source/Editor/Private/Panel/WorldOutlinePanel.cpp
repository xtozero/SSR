#include "WorldOutlinePanel.h"

#include "IEditor.h"
#include "imgui.h"
#include "PanelFactory.h"
#include "PanelSharedContext.h"
#include "World/World.h"

using ::logic::World;

namespace editor
{
	REGISTER_PANEL( WorldOutlinePanel );
	void WorldOutlinePanel::Draw()
	{
		IEditor& editor = GetEditor();
		PanelSharedContext& sharedCtx = editor.GetPanelSharedCtx();

		ImGui::Begin( "World Outliner" );
		{
			World& world = editor.GetWorld();
			auto& gameObjects = world.GameObjects();

			int32 n = 0;
			for ( auto& object : gameObjects )
			{
				if ( object->GetName().empty() )
				{
					continue;
				}

				ImGui::PushID( n++ );
				if ( ImGui::Selectable( object->GetName().c_str(), sharedCtx.IsSelectedObject( object.get() ) ) )
				{
					sharedCtx.UnselectObject();
					sharedCtx.SelectObject( object.get() );
				}
				ImGui::PopID();
			}
		}
		ImGui::End();
	}

	void WorldOutlinePanel::HandleUserInput( [[maybe_unused]] const engine::UserInput& input )
	{
	}
}
