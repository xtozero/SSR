#include "ScenePanel.h"

#include "Core/ILogic.h"
#include "GameObject/HitObject.h"
#include "GraphicsResource/Viewport.h"
#include "IEditor.h"
#include "LibraryTool/InterfaceFactories.h"
#include "PanelFactory.h"
#include "PanelSharedContext.h"
#include "Renderer/RenderView.h"
#include "Scene/GameClientViewport.h"
#include "World/World.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>

using ::engine::UserInput;
using ::engine::UserInputCode;
using ::logic::GameClientViewport;

namespace
{
    bool IsMouseClick( const UserInput& input )
    {
        return ( input.m_code == UserInputCode::UIC_MOUSE_LEFT ) 
            || ( input.m_code == UserInputCode::UIC_MOUSE_RIGHT )
            || ( input.m_code == UserInputCode::UIC_MOUSE_MIDDLE );
    }

    bool IsRightMouseRelease( const UserInput& input )
    {
        return ( input.m_code == UserInputCode::UIC_MOUSE_RIGHT )
            && ( input.m_axis[2] > 0 );
    }

    bool CanPickObject( const UserInput& input )
    {
        return ( input.m_code == UserInputCode::UIC_MOUSE_LEFT )
            && ( input.m_axis[2] < 0 )
            && input.m_buttonState.IsButtonReleased( UserInputCode::UIC_MOUSE_RIGHT );
    }
}

namespace editor
{
    REGISTER_PANEL( ScenePanel );
	void ScenePanel::Draw()
	{
        IEditor& editor = GetEditor();
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
            ImVec2 panelMin = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
            ImVec2 panelMax = ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos();
            m_panelArea = {
                .m_left = panelMin.x,
                .m_top = panelMin.y,
                .m_right = panelMax.x,
                .m_bottom = panelMax.y
            };

			m_passingInputToLogic = ( ImGui::IsWindowHovered() && ImGui::IsWindowFocused() && ImGui::IsWindowDocked() );

            ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
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

        IEditor& editor = GetEditor();
        engine::UserInput inputForLogic = input;
        if ( IsMouseClick( input ) )
        {
            inputForLogic.m_axis[0] = std::clamp( inputForLogic.m_axis[0], m_panelArea.m_left, m_panelArea.m_right );
            inputForLogic.m_axis[1] = std::clamp( inputForLogic.m_axis[1], m_panelArea.m_top, m_panelArea.m_bottom );

            inputForLogic.m_axis[0] -= m_panelArea.m_left;
            inputForLogic.m_axis[1] -= m_panelArea.m_top;

            if ( CanPickObject( input ) )
            {
                auto x = static_cast<uint32>( inputForLogic.m_axis[0] );
                auto y = static_cast<uint32>( inputForLogic.m_axis[1] );

				rendercore::HitProxy* hitProxy = nullptr;
				if ( GameClientViewport* gameClientViewport = editor.GetGameClientViewport() )
				{
					hitProxy = gameClientViewport->GetHitProxy( x, y );
				}

                PanelSharedContext& sharedCtx = editor.GetPanelSharedCtx();
                sharedCtx.UnselectObject();

                if ( auto hitObject = Cast<logic::HitObject>( hitProxy ) )
                {
                    sharedCtx.SelectObject( hitObject->GetObject() );
                }
			}
			else if ( IsRightMouseRelease( input ) )
            {
                if ( GameClientViewport* gameClientViewport = editor.GetGameClientViewport() )
                {
                    gameClientViewport->InvalidateHitProxy();
                }
            }
        }

        if ( logic )
        {
            logic->HandleUserInput( inputForLogic );
        }
    }
}
