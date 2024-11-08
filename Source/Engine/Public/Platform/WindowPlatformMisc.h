#pragma once

#include "LibraryTool/Common.h"

namespace engine
{
	struct WindowPlatformMisc
	{
		ENGINE_DLL static bool LaunchApplication( const char* operation, const char* file, const char* param = nullptr );
	};

	using PlatformMisc = WindowPlatformMisc;
}