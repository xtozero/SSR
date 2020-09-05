#pragma once

#include "Core/IEngine.h"
#include "WindowPlatformInputConvertor.h"

#include <memory>

class IFileSystem;
class ILogic;
class ITaskScheduler;

class WindowPlatformEngine : public IEngine
{
public:
	virtual bool BootUp( IPlatform& ) override;

	virtual void Run( ) override;

	virtual LRESULT MsgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	~WindowPlatformEngine( );

protected:
	virtual void ProcessInput( ) override;
	virtual bool IsAvailable( ) override { return m_isAvailable; }

private:
	void Shutdown( );

	WindowPlatformInputConvertor m_inputConvertor;
	HMODULE m_logicDll = nullptr;
	ILogic* m_logic = nullptr;
	IPlatform* m_platform = nullptr;
	IFileSystem* m_fileSystem = nullptr;
	ITaskScheduler* m_taskScheduler = nullptr;

	bool m_isAvailable = false;
	bool m_isPaused = false;
	bool m_maximized = false;
	bool m_minimized = false;
	bool m_resizing = false;
};

