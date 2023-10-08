#pragma once

namespace engine
{
	struct UserInput;
}

namespace editor
{
	class IEditor;

	class IPanel
	{
	public:
		virtual void Draw( IEditor& editor ) = 0;
		virtual void HandleUserInput( const engine::UserInput& input ) = 0;

		virtual ~IPanel() = default;
	};
}
