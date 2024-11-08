#include "WindowPlatformMisc.h"

bool engine::WindowPlatformMisc::LaunchApplication( const char* operation, const char* file, const char* param )
{
	HINSTANCE result = ShellExecuteA( nullptr, operation, file, param, nullptr, SW_SHOWNORMAL );
	return result > (HINSTANCE)32;
}
