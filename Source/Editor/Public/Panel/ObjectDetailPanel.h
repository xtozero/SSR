#pragma once

#include "IPanel.h"

namespace editor
{
	class ObjectDetailPanel final : public IPanel
	{
		virtual void Draw( IEditor& editor ) override;
		virtual void HandleUserInput( const engine::UserInput& input ) override;
	};
}
