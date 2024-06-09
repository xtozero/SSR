#pragma once

#include "Core/IEngine.h"
#include "SizedTypes.h"
#include "WindowPlatformInputConvertor.h"

#include <memory>

class IFileSystem;
class ITaskScheduler;

namespace logic
{
	class ILogic;
}

namespace engine
{
	class WindowPlatformEngine : public IEngine
	{
	public:
		virtual bool BootUp( IPlatform& platform, const char* argv ) override;

		virtual void Run() override;

		virtual void ProcessInput() override;

		ENGINE_DLL LRESULT MsgProc( HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam );

		virtual ~WindowPlatformEngine() override;

	protected:
		virtual bool IsAvailable() override { return m_isAvailable; }

	private:
		void Shutdown();

		WindowPlatformInputConvertor m_inputConvertor;
		HMODULE m_logicDll = nullptr;
		logic::ILogic* m_logic = nullptr;
		IPlatform* m_platform = nullptr;
		IFileSystem* m_fileSystem = nullptr;
		ITaskScheduler* m_taskScheduler = nullptr;

		bool m_isAvailable = false;
		bool m_isPaused = false;
		bool m_maximized = false;
		bool m_minimized = false;
		bool m_resizing = false;
	};
}
