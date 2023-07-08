#pragma once

#include "AppConfig/AppConfig.h"

class DefaultApp : public ConfigBase<DefaultApp, "default_engine">
{
	GENERATE_CLASS_TYPE_INFO( DefaultApp );

public:
	static bool IsEditor();

private:
	PROPERTY( editor )
	bool m_editor = false;
};