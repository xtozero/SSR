#pragma once

#include "IPanel.h"
#include "SizedTypes.h"

#include <utility>

namespace rendercore
{
	class HitProxy;
}

namespace editor
{
	class ScenePanel final : public Panel
	{
		using Panel::Panel;

	public:
		virtual void Draw() override;
		virtual void HandleUserInput( const engine::UserInput& input ) override;

	private:
		RectangleArea<float> m_panelArea = {};
		std::pair<uint32, uint32> m_viewportResolution = {};
		bool m_passingInputToLogic = false;
	};
}