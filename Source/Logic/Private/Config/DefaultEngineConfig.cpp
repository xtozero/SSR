#include "Config/DefaultEngineConfig.h"

#include "AppConfig/AppConfig.h"
#include "Ini/Ini.h"
#include "LibraryTool/InterfaceFactories.h"

#include <cassert>

using ::ini::Ini;
using ::ini::Section;

const char* DefaultEngine::GetDefaultWorld() const
{
	return m_defaultWorld.c_str();
}
