#include "Config/DefaultLogicConfig.h"

#include "AppConfig/AppConfig.h"
#include "Ini/Ini.h"
#include "LibraryTool/InterfaceFactories.h"

#include <cassert>

using ::ini::Ini;
using ::ini::Section;

const char* DefaultLogic::GetDefaultWorld()
{
	return GetInstance().m_defaultWorld.c_str();
}

const float4& DefaultLogic::GetDefaultBackgroundColor()
{
	return GetInstance().m_defaultBackgroundColor;
}
