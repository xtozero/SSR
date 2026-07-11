#pragma once

#include "IPanel.h"

namespace editor
{
	class ObjectDetailPanel final : public Panel
	{
		using Panel::Panel;

	public:
		virtual void Draw() override;
	};
}
