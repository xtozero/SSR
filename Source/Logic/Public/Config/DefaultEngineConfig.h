#pragma once

#include "AppConfig/AppConfig.h"

#include <string>

class DefaultEngine : public ConfigBase<DefaultEngine, "default_engine">
{
	GENERATE_CLASS_TYPE_INFO( DefaultEngine );

public:
	const char* GetDefaultWorld() const;

private:
	PROPERTY( defaultWorld );
	std::string m_defaultWorld;
};
