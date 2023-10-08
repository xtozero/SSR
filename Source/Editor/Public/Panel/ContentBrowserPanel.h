#pragma once

#include "IPanel.h"
#include "SizedTypes.h"

#include <filesystem>

namespace editor
{
	class ContentBrowserPanel final : public IPanel
	{
	public:
		virtual void Draw( IEditor& editor ) override;
		virtual void HandleUserInput( const engine::UserInput& input ) override;

		ContentBrowserPanel();

	private:
		void DrawContentDirectoryTree( std::filesystem::path current );
		void DrawCurrentDirectoryFiles();
		void DrawCurrentDirectoryPath( std::filesystem::path relative, int32 depth = 0 );

		std::filesystem::path m_curDirectory;
	};
}