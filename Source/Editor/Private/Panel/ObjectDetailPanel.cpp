#include "ObjectDetailPanel.h"

#include "Components/StaticMeshComponent.h"
#include "GameObject/GameObject.h"
#include "IEditor.h"
#include "imgui.h"
#include "Material.h"
#include "Math/Rotator.h"
#include "PanelFactory.h"
#include "PanelSharedContext.h"

#include <numeric>

using ::DirectX::XMConvertToRadians;
using ::logic::Component;
using ::logic::StaticMeshComponent;
using ::rendercore::FloatProperty;
using ::rendercore::Float4Property;
using ::rendercore::Material;

namespace editor
{
	void DrawProperty( logic::CGameObject& object, const Transform& transform )
	{
		const Vector& position = transform.GetTranslation();
		const Vector& scale = transform.GetScale3D();
		const Rotator& rotation = transform.GetRotation().ToRotator();

		float3 fPosition = { position[0], position[1], position[2] };
		float3 fScale = { scale[0], scale[1], scale[2] };
		float3 fRotation = { rotation[0], rotation[1], rotation[2] };

		if ( ImGui::BeginTable( "transform", 2, ImGuiTableFlags_SizingFixedFit ) )
		{
			// Position
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex( 0 );
				ImGui::Text( "Position" );

				ImGui::TableSetColumnIndex( 1 );
				ImGui::PushItemWidth( -FLT_MIN );
				if ( ImGui::DragFloat3( "Position", fPosition, 0.1f ) )
				{
					object.SetPosition( Vector( fPosition ) );
				}
			}

			// Scale
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex( 0 );
				ImGui::Text( "Scale" );

				ImGui::TableSetColumnIndex( 1 );
				if ( ImGui::DragFloat3( "Scale", fScale, 0.1f ) )
				{
					object.SetScale3D( Vector( fScale ) );
				}
			}

			// Rotation
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex( 0 );
				ImGui::Text( "Rotation" );

				ImGui::TableSetColumnIndex( 1 );
				if ( ImGui::DragFloat3( "Rotation", fRotation, 0.1f ) )
				{
					object.SetRotation( Rotator( fRotation[0], fRotation[1], fRotation[2] ).ToQuat() );
				}
			}

			ImGui::EndTable();
		}
	}

	void DrawProperty( Component& owner, Material& material )
	{
		bool bRenderStateDirty = false;

		if ( ImGui::BeginTable( "materials", 2, ImGuiTableFlags_SizingFixedFit ) )
		{
			const auto& properties = material.GetProperties();
			for ( auto& [name, property] : properties )
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex( 0 );
				ImGui::Text( name.CStr() );

				ImGui::TableSetColumnIndex( 1 );
				ImGui::PushItemWidth( -FLT_MIN );
				if ( auto* floatProperty = Cast<FloatProperty>( property.get() ) )
				{
					float value = floatProperty->Value();
					if ( ImGui::DragFloat( name.CStr(), &value, 0.01f ) )
					{
						floatProperty->SetValue( value );
						bRenderStateDirty = true;
					}
				}
				else if ( auto float4Property = Cast<Float4Property>( property.get() ) )
				{
					const Vector4& oldValue = float4Property->Value();
					float4 value = { oldValue.x, oldValue.y, oldValue.z, oldValue.w };

					if ( ImGui::DragFloat4( name.CStr(), value, 0.01f ) )
					{
						float4Property->SetValue( Vector4( value ) );
						bRenderStateDirty = true;
					}
				}
				ImGui::PopItemWidth();
			}

			ImGui::EndTable();
		}

		if ( bRenderStateDirty )
		{
			owner.MarkRenderStateDirty();
		}
	}

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
					logic::CGameObject* object = *objects.begin();

					ImGui::BeginChild( "Object Properties" );
					{
						ImGui::SeparatorText( "Transform" );

						DrawProperty( *object, object->GetTransform() );
					}

					{
						ImGui::SeparatorText( "Material" );

						std::vector<StaticMeshComponent*> components;
						object->GetComponents( components );

						for ( StaticMeshComponent* component : components )
						{
							uint32 numMaterials = component->NumMaterials();
							for ( uint32 i = 0; i < numMaterials; ++i )
							{
								DrawProperty( *component, *component->GetMaterial( i ) );
							}
						}
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
