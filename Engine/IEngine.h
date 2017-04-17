#pragma once

#include "common.h"
#include "../shared/Util.h"

#include <memory>

class IPlatform;

class IEngine
{
public:
	virtual bool BootUp( IPlatform& ) = 0;
	virtual void ShutDown( ) = 0;

	virtual void ProcessInput( ) = 0;

	virtual void Run( ) = 0;

	virtual ~IEngine( ) = default;
};

namespace SUPPORT_PLATFORM
{
	struct Window {};
}

ENGINE_FUNC_DLL Owner<IEngine*> CreatePlatformEngine( SUPPORT_PLATFORM::Window );