#pragma once

#include "IPanel.h"
#include "SizedTypes.h"

#include <optional>

namespace editor
{
	class WorldOutlinePanel final : public IPanel
	{
	public:
		virtual void Draw( IEditor& editor ) override;
		virtual void HandleUserInput( const engine::UserInput& input ) override;
	};
}
