#pragma once

#include "IEditor.h"

class ILogic;

class ImguiEditor final : public IEditor
{
public:
	virtual bool BootUp( IPlatform& platform ) override;
	virtual void Update() override;
	virtual void Pause() override;
	virtual void Resume() override;
	virtual void HandleUserInput( const UserInput& input ) override;
	virtual void HandleTextInput( uint64 text, bool bUnicode ) override;
	virtual void AppSizeChanged( IPlatform& platform ) override;

	~ImguiEditor();

private:
	void DrawDockSpace();
	void DrawMainMenuBar();

	HMODULE m_logicDll = nullptr;
	ILogic* m_logic = nullptr;
};