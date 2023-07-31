#pragma once

#include "AppConfig/AppConfig.h"
#include "SizedTypes.h"

#include <utility>

class DefaultApp : public ConfigBase<DefaultApp, "default_engine">
{
	GENERATE_CLASS_TYPE_INFO( DefaultApp );

public:
	static bool IsEditor();
	static std::pair<int32, int32> GetClientSize();

private:
	PROPERTY( editor )
	bool m_editor = false;

	PROPERTY( clientWidth )
	int32 m_clientWidth;

	PROPERTY( clientHeight )
	int32 m_clientHeight;
};