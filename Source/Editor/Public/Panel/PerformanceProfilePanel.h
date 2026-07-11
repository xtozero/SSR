#pragma once

#include "IPanel.h"

namespace editor
{
	class PerformanceProfilePanel final : public Panel
	{
		using Panel::Panel;

	public:
		virtual void Draw() override;
	};
}