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
	virtual void AppSizeChanged( IPlatform& platform ) override;

	~ImguiEditor();

private:
	HMODULE m_logicDll = nullptr;
	ILogic* m_logic = nullptr;
};