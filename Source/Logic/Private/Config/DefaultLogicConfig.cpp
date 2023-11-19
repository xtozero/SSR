#include "Config/DefaultLogicConfig.h"

#include "AppConfig/AppConfig.h"
#include "Ini/Ini.h"
#include "LibraryTool/InterfaceFactories.h"

#include <cassert>

using ::ini::Ini;
using ::ini::Section;

namespace logic
{
	const char* DefaultLogic::GetDefaultWorld()
	{
		return GetInstance().m_defaultWorld.c_str();
	}

	void DefaultLogic::SetDefaultWorld( const std::filesystem::path& worldPath )
	{
		std::filesystem::path relative = std::filesystem::relative( worldPath, std::filesystem::current_path() );
		GetInstance().m_defaultWorld = relative.generic_string();
	}

	const float4& DefaultLogic::GetDefaultBackgroundColor()
	{
		return GetInstance().m_defaultBackgroundColor;
	}

	const bool DefaultLogic::IsSaveLastWorldAsDefault()
	{
		return GetInstance().m_saveLastWorldAsDefault;
	}
}
