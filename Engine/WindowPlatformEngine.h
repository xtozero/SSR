#pragma once

#include "IEngine.h"
#include "WindowPlatformInputConvertor.h"

#include <memory>

class ILogic;

class WindowPlatformEngine : public IEngine
{
public:
	virtual bool BootUp( IPlatform& ) override;
	virtual void ShutDown( ) override;

	virtual void Run( ) override;

	virtual LRESULT MsgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

protected:
	virtual void ProcessInput( ) override;
	virtual bool IsAvailable( ) override { return m_isAvailable; }

private:

	WindowPlatformInputConvertor m_inputConvertor;
	HMODULE m_logicDll = nullptr;
	std::unique_ptr<ILogic> m_logic = nullptr;
	IPlatform* m_platform = nullptr;

	bool m_isAvailable = false;
	bool m_isPaused = false;
	bool m_maximized = false;
	bool m_minimized = false;
	bool m_resizing = false;
};

