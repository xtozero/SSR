#pragma once

#include "IPanel.h"
#include "SizedTypes.h"

#include <optional>

namespace editor
{
	class WorldOutlinePanel final : public Panel
	{
		using Panel::Panel;

	public:
		virtual void Draw() override;
		virtual void HandleUserInput( const engine::UserInput& input ) override;
	};
}
