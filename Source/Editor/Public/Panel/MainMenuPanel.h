#pragma once

#include "IPanel.h"

namespace editor
{
	class MainMenuPanel final : public Panel
	{
		using Panel::Panel;

	public:
		virtual void Draw() override;
		virtual void HandleUserInput( const engine::UserInput& input ) override;

	private:
	};
}
