#pragma once

#include "IPanel.h"

namespace editor
{
	class PerformanceProfilePanel final : public IPanel
	{
	public:
		virtual void Draw( IEditor& editor ) override;
		virtual void HandleUserInput( [[maybe_unused]] const engine::UserInput& input ) override {}
	};
}