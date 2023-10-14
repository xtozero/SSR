#include "ContentBrowserPanel.h"

#include "IEditor.h"
#include "imgui.h"
#include "PanelFactory.h"
#include "PanelSharedContext.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace editor
{
    REGISTER_PANEL( ContentBrowserPanel );
	void ContentBrowserPanel::Draw( [[maybe_unused]] IEditor& editor )
	{
		ImGui::Begin( "Content Directory Outliner" );
        {
            DrawContentDirectoryTree( fs::current_path() );
        }
		ImGui::End();

		ImGui::Begin( "Content Browser" );
        {
            DrawCurrentDirectoryPath( m_curDirectory );

            ImGui::Separator();

            DrawCurrentDirectoryFiles( editor );
        }
		ImGui::End();
	}

	void ContentBrowserPanel::HandleUserInput( [[maybe_unused]] const engine::UserInput& input )
	{
	}

	ContentBrowserPanel::ContentBrowserPanel()
		: m_curDirectory( fs::current_path() )
	{
	}

    void ContentBrowserPanel::DrawContentDirectoryTree( fs::path current )
    {
        if ( fs::is_directory( current ) == false )
        {
            return;
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow 
                                | ImGuiTreeNodeFlags_OpenOnDoubleClick 
                                | ImGuiTreeNodeFlags_SpanAvailWidth;
        if ( m_curDirectory == current )
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        std::string relative = fs::relative( m_curDirectory, current ).generic_string();
        bool isSubDirectory = relative.starts_with( ".." ) == false;
        if ( isSubDirectory )
        {
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        bool nodeOpen = ImGui::TreeNodeEx( current.filename().generic_string().c_str(), flags );
        if ( ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen() )
        {
            m_curDirectory = current;
        }

        if ( nodeOpen )
        {
            for ( const auto& child : fs::directory_iterator( current ) )
            {
                DrawContentDirectoryTree( child );
            }

            ImGui::TreePop();
        }
    }

    void ContentBrowserPanel::DrawCurrentDirectoryFiles( IEditor& editor )
    {
        ImGui::BeginChild("Current Directory Files");
        {
            const ImVec2 iconSize( 100, 100 );
            size_t numFiles = std::distance( fs::directory_iterator( m_curDirectory ), fs::directory_iterator{} );

            int32 n = 0;
            ImGuiStyle& style = ImGui::GetStyle();
            float windowVisibleX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
            for ( const auto& entry : fs::directory_iterator( m_curDirectory ) )
            {
				fs::path filename = entry.path().filename();
				ImGui::BeginChild( ( std::format( "child##{}", n ) ).c_str(), iconSize, true, ImGuiWindowFlags_NoScrollbar );
                {
                    ImGui::Button( filename.generic_string().c_str(), ImGui::GetContentRegionAvail() );
                    ImGui::SetItemTooltip( filename.generic_string().c_str() );
                    if ( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
                    {
                        if ( entry.is_directory() )
                        {
                            m_curDirectory = entry.path();
                        }
                        else if ( entry.is_regular_file() )
                        {
                            OpenContent( editor, entry );
                        }
                    }
                }
                ImGui::EndChild();

                float lastButtonX = ImGui::GetItemRectMax().x;
                float nextButtonX = lastButtonX + style.ItemSpacing.x + iconSize.x;
                if ( ( n + 1 ) < numFiles && nextButtonX < windowVisibleX )
                {
                    ImGui::SameLine();
                }

                ++n;
            }
        }
        ImGui::EndChild();
    }

    void ContentBrowserPanel::DrawCurrentDirectoryPath( fs::path current, int32 depth )
    {
        if ( current == fs::current_path().parent_path() )
        {
            return;
        }

        DrawCurrentDirectoryPath( current.parent_path(), depth + 1 );

        ImGui::SameLine();
        ImGui::PushID( depth );
        ImGui::PushStyleColor( ImGuiCol_Button, (ImVec4)ImColor(0.f, 0.f, 0.f, 0.f) );
        ImGui::PushStyleColor( ImGuiCol_ButtonHovered, (ImVec4)ImColor( 0.f, 0.f, 0.f, 0.f ) );
        ImGui::PushStyleColor( ImGuiCol_ButtonActive, (ImVec4)ImColor( 0.f, 0.f, 0.f, 0.f ) );
        ImGui::ArrowButton( "Directory Seperator", ImGuiDir_Right);
        ImGui::PopStyleColor( 3 );
        ImGui::PopID();

        ImGui::SameLine();
        ImGui::PushStyleColor( ImGuiCol_Button, (ImVec4)ImColor( 0.f, 0.f, 0.f, 0.f ) );
        if ( ImGui::Button( current.filename().generic_string().c_str() ) )
        {
            m_curDirectory = current;
        }
        ImGui::PopStyleColor( 1 );
    }

    void ContentBrowserPanel::OpenContent( IEditor& editor, std::filesystem::path file )
    {
        if ( file.extension() == ".json" )
        {
            editor.GetPanelSharedCtx().UnselectObject();

            // Temporarily assume all JSON files are world files
            editor.LoadWorld( file.generic_string().c_str() );
        }
    }
}
