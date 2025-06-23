#pragma once

#include "LibraryTool/Common.h"

#include <filesystem>

namespace engine
{
	struct WindowPlatformProcess
	{
		ENGINE_DLL static bool LaunchApplication( const char* operation, const char* file, const char* param = nullptr, const char* workingDirectory = nullptr, bool needWait = false );

		ENGINE_DLL static std::filesystem::path GetExecutablePath();
		ENGINE_DLL static std::filesystem::path GetExecutableDirectory();

		ENGINE_DLL static std::filesystem::path GetRootDirectory();

		ENGINE_DLL static std::filesystem::path GetWorkingDirectory();
	};

	using PlatformProcess = WindowPlatformProcess;
}