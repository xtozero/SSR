#include "ObjectDetailPanel.h"

#include "GameObject/GameObject.h"
#include "IEditor.h"
#include "imgui.h"
#include "Math/Rotator.h"
#include "PanelFactory.h"
#include "PanelSharedContext.h"

#include <numeric>

using ::DirectX::XMConvertToRadians;

namespace editor
{
	REGISTER_PANEL( ObjectDetailPanel );
	void ObjectDetailPanel::Draw( IEditor& editor )
	{
		ImGui::Begin( "Object Detail" );
		{
			PanelSharedContext& sharedCtx = editor.GetPanelSharedCtx();
			const auto& objects = sharedCtx.GetSelectedObjects();
			if ( objects.empty() == false )
			{
				ImVec2 contentRegion = ImGui::GetContentRegionAvail();

				// Component Hierarchy
				{
					ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 5.f );
					ImGui::BeginChild("Component Hierarchy", ImVec2( 0, contentRegion.y * 0.4f ), true);
					{

					}
					ImGui::EndChild();
					ImGui::PopStyleVar();
				}

				ImGui::Separator();

				// Object Properties
				{
					ImGui::BeginChild( "Object Properties" );
					{
						ImGui::SeparatorText( "Transform" );

						constexpr float min = std::numeric_limits<float>::lowest() * 0.5f;
						constexpr float max = std::numeric_limits<float>::max() * 0.5f;

						logic::CGameObject* object = *objects.begin();

						const Vector& position = object->GetPosition();
						const Vector& scale = object->GetScale3D();
						const Rotator& rotation = object->GetRotation().ToRotator();

						float fPosition[3] = { position[0], position[1], position[2] };
						float fScale[3] = { scale[0], scale[1], scale[2] };
						float fRotation[3] = { rotation[0], rotation[1], rotation[2] };

						ImGui::PushID( "Position" );
						ImGui::Text( "Position" );
						ImGui::SameLine();
						if ( ImGui::DragFloat3( "", fPosition, 0.1f, min, max ) )
						{
							object->SetPosition( Vector( fPosition ) );
						}
						ImGui::PopID();

						ImGui::PushID( "Scale" );
						ImGui::Text( "Scale   " );
						ImGui::SameLine();
						if ( ImGui::DragFloat3( "", fScale, 0.1f, min, max ) )
						{
							object->SetScale3D( Vector( fScale ) );
						}
						ImGui::PopID();

						ImGui::PushID( "Rotation" );
						ImGui::Text( "Rotation" );
						ImGui::SameLine();
						if ( ImGui::DragFloat3( "", fRotation, 0.1f, min, max ) )
						{
							object->SetRotation( Quaternion( XMConvertToRadians( fRotation[0] ), XMConvertToRadians( fRotation[1] ), XMConvertToRadians( fRotation[2] ) ) );
						}
						ImGui::PopID();
					}
					ImGui::EndChild();
				}
			}
		}
		ImGui::End();
	}

	void ObjectDetailPanel::HandleUserInput( const engine::UserInput& input )
	{
	}
}
