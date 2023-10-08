#include "ScenePanel.h"

#include "Core/ILogic.h"
#include "GraphicsResource/Viewport.h"
#include "IEditor.h"
#include "imgui.h"
#include "LibraryTool/InterfaceFactories.h"
#include "PanelFactory.h"
#include "Scene/GameClientViewport.h"

using ::logic::GameClientViewport;

namespace editor
{
    REGISTER_PANEL( ScenePanel );
	void ScenePanel::Draw( IEditor& editor )
	{
        GameClientViewport* gameClientViewport = editor.GetGameClientViewport();
        if ( gameClientViewport == nullptr )
        {
            return;
        }

        rendercore::Viewport* viewport = gameClientViewport->GetViewport();
        if ( viewport == nullptr
            || viewport->Texture() == nullptr )
        {
            return;
        }

        ImGui::Begin( "Scene" );
        {
			m_passingInputToLogic = ( ImGui::IsWindowHovered() && ImGui::IsWindowFocused() && ImGui::IsWindowDocked() );

			auto contentRegionAvail = ImGui::GetContentRegionAvail();
			if ( contentRegionAvail.x > 0 && contentRegionAvail.y > 0 )
			{
                std::pair<uint32, uint32> viewportResolution = {
                    static_cast<uint32>( contentRegionAvail.x ),
                    static_cast<uint32>( contentRegionAvail.y )
                };

				if ( m_viewportResolution != viewportResolution )
				{
					viewport->Resize( viewportResolution );
					m_viewportResolution = viewportResolution;
				}
                else
                {
                    ImGui::Image( (ImTextureID)viewport->Texture(), contentRegionAvail );
                }
			}
        }
        ImGui::End();
	}

    void ScenePanel::HandleUserInput( const engine::UserInput& input )
    {
        static auto* logic = GetInterface<logic::ILogic>();
        if ( m_passingInputToLogic == false )
        {
            return;
        }

        if ( logic )
        {
            logic->HandleUserInput( input );
        }
    }
}
