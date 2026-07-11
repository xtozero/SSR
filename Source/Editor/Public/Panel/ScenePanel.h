#pragma once

#include "ConsoleInputWidget.h"
#include "IPanel.h"
#include "SizedTypes.h"

#include <utility>

namespace engine
{
	class IPlatform;
}

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
		virtual void HandleUserInput( engine::IPlatform& platform, const engine::UserInput& input ) override;

	private:
		RectangleArea<float> m_panelArea = {};
		std::pair<uint32, uint32> m_viewportResolution = {};

		bool m_passingInputToLogic = false;

		ConsoleInputWidget m_consoleInputWidget;
	};
}