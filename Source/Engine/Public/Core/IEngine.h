#pragma once

#include "common.h"
#include "GuideTypes.h"
#include "SizedTypes.h"

#include <memory>

class IPlatform;

class IEngine
{
public:
	virtual bool BootUp( IPlatform& platform, char* argv ) = 0;

	virtual void Run() = 0;

	virtual LRESULT MsgProc( HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam ) = 0;

	virtual ~IEngine() = default;

protected:
	virtual void ProcessInput() = 0;
	virtual bool IsAvailable() = 0;
};

namespace SUPPORT_PLATFORM
{
	struct Window {};
}

Owner<IEngine*> CreatePlatformEngine();
void DestroyPlatformEngine( Owner<IEngine*> pEngine );