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
		virtual void Draw() = 0;
		virtual void HandleUserInput( const engine::UserInput& input ) = 0;

		virtual ~IPanel() = default;
	};

	class Panel : public IPanel
	{
	public:
		Panel( IEditor& editor )
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
