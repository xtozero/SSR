#pragma once

#include "AppConfig/AppConfig.h"

#include <string>

class DefaultEngine : public ConfigBase<DefaultEngine, "default_engine">
{
	GENERATE_CLASS_TYPE_INFO( DefaultEngine );

public:
	static const char* GetDefaultWorld();

private:
	PROPERTY( defaultWorld );
	std::string m_defaultWorld;
};
