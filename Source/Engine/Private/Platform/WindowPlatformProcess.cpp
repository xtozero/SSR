#include "WindowPlatformProcess.h"

namespace fs = std::filesystem;

bool engine::WindowPlatformProcess::LaunchApplication( const char* operation, const char* file, const char* param, const char* workingDirectory, bool needWait )
{
	SHELLEXECUTEINFOA shellExecuteInfo = {
		.cbSize = sizeof(SHELLEXECUTEINFOA),
		.fMask = SEE_MASK_NOCLOSEPROCESS,
		.hwnd = nullptr,
		.lpVerb = operation,
		.lpFile = file,
		.lpParameters = param,
		.lpDirectory = workingDirectory,
		.nShow = SW_SHOWNORMAL
	};

	if ( ShellExecuteExA( &shellExecuteInfo ) )
	{
		if ( needWait )
		{
			if ( shellExecuteInfo.hProcess != nullptr )
			{
				WaitForSingleObject( shellExecuteInfo.hProcess, INFINITE );
				CloseHandle( shellExecuteInfo.hProcess );
			}
		}

		return true;
	}

	return false;
}

void engine::WindowPlatformProcess::ExitProcess()
{
	PostMessageA( nullptr, WM_QUIT, 0, 0 );
}

fs::path engine::WindowPlatformProcess::GetExecutablePath()
{
	static char path[MAX_PATH] = {};

	if (path[0] == '\0')
	{
		if ( !GetModuleFileNameA( nullptr, path, std::extent_v<decltype(path)> ) )
		{
			path[0] = '\0';
		}
	}

	return fs::path( path );
}

fs::path engine::WindowPlatformProcess::GetExecutableDirectory()
{
	static fs::path directoryPath;

	if ( directoryPath.empty() )
	{
		directoryPath = GetExecutablePath();
		directoryPath = directoryPath.parent_path();
	}

	return directoryPath;
}

std::filesystem::path engine::WindowPlatformProcess::GetRootDirectory()
{
	return GetWorkingDirectory().parent_path();
}

fs::path engine::WindowPlatformProcess::GetWorkingDirectory()
{
	return fs::current_path();
}
