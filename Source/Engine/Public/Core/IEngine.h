#pragma once

#include "common.h"
#include "Util.h"

#include <memory>

class IPlatform;

class IEngine
{
public:
	virtual bool BootUp( IPlatform& ) = 0;
	virtual void ShutDown( ) = 0;

	virtual void Run( ) = 0;

	virtual ~IEngine( ) = default;

protected:
	virtual void ProcessInput( ) = 0;
	virtual bool IsAvailable( ) = 0;
};

namespace SUPPORT_PLATFORM
{
	struct Window {};
}

ENGINE_FUNC_DLL Owner<IEngine*> CreatePlatformEngine( );
ENGINE_FUNC_DLL void DestroyPlatformEngine( IEngine* pEngine );