#pragma once

#include "IPanel.h"

namespace editor
{
	class PipelineStatisticsPanel final : public Panel
	{
		using Panel::Panel;

	public:
		virtual void Draw() override;
		virtual void HandleUserInput( [[maybe_unused]] const engine::UserInput& input ) override {}
	};
}