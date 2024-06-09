#pragma once

#include "IPanel.h"
#include "SizedTypes.h"

#include <filesystem>

namespace editor
{
	class ContentBrowserPanel final : public Panel
	{
	public:
		virtual void Draw() override;
		virtual void HandleUserInput( const engine::UserInput& input ) override;

		ContentBrowserPanel( IEditor& editor );

	private:
		void DrawContentDirectoryTree( std::filesystem::path current );
		void DrawCurrentDirectoryFiles();
		void DrawCurrentDirectoryPath( std::filesystem::path relative, int32 depth = 0 );

		void OpenContent(std::filesystem::path file );

		std::filesystem::path m_curDirectory;
	};
}