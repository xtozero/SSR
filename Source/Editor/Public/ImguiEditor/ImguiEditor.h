#pragma once

#include "IEditor.h"
#include "IPanel.h"

#include <memory>
#include <vector>

namespace logic
{
	class GameClientViewport;
	class ILogic;
}

namespace editor
{
	class ImguiEditor final : public IEditor
	{
	public:
		virtual bool BootUp( engine::IPlatform& platform ) override;
		virtual void Update() override;
		virtual void Pause() override;
		virtual void Resume() override;
		virtual void HandleUserInput( const engine::UserInput& input ) override;
		virtual void HandleTextInput( uint64 text, bool bUnicode ) override;
		virtual void AppSizeChanged( engine::IPlatform& platform ) override;
		virtual logic::GameClientViewport * GetGameClientViewport() override;
		virtual bool LoadWorld( const char* filePath ) override;
		virtual void UnloadWorld() override;

		~ImguiEditor();

	private:
		HMODULE m_logicDll = nullptr;
		ILogic* m_logic = nullptr;

		std::vector<std::unique_ptr<IPanel>> m_panels;
	};
}
