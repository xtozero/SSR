#pragma once

#include "IPanel.h"
#include "SizedTypes.h"

#include <utility>

namespace editor
{
	class ScenePanel final : public IPanel
	{
	public:
		virtual void Draw( IEditor& editor ) override;
		virtual void HandleUserInput( const engine::UserInput& input ) override;

	private:
		bool m_passingInputToLogic = false;
		std::pair<uint32, uint32> m_viewportResolution = {};
	};
}