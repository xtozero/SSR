#pragma once

namespace engine
{
	class IPlatform;

	struct UserInput;
}

namespace editor
{
	class IEditor;

	class IPanel
	{
	public:
		virtual void Draw() = 0;
		virtual void HandleUserInput( engine::IPlatform& platform, const engine::UserInput& input ) = 0;

		virtual ~IPanel() = default;
	};

	class Panel : public IPanel
	{
	public:
		virtual void HandleUserInput( [[maybe_unused]] engine::IPlatform& platform,
		                              [[maybe_unused]] const engine::UserInput& input ) override
		{}

		explicit Panel( IEditor& editor )
			: m_editor( editor )
		{}

	protected:
		IEditor& GetEditor() const
		{
			return m_editor;
		}

	private:
		IEditor& m_editor;
	};
}
