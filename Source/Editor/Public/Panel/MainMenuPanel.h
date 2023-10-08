#pragma once

#include "IPanel.h"

namespace editor
{
	class MainMenuPanel final : public IPanel
	{
	public:
		virtual void Draw( IEditor& editor ) override;
		virtual void HandleUserInput( const engine::UserInput& input ) override;

	private:
	};
}
