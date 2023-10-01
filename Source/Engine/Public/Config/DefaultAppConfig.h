#pragma once

#include "AppConfig/AppConfig.h"
#include "SizedTypes.h"

#include <utility>

namespace engine
{
	class DefaultApp : public ConfigBase<DefaultApp, "default_engine">
	{
		GENERATE_CLASS_TYPE_INFO( DefaultApp );

	public:
		ENGINE_DLL static bool IsEditor();
		static std::pair<int32, int32> GetClientSize();

	private:
		PROPERTY( editor )
		bool m_editor = false;

		PROPERTY( clientWidth )
		int32 m_clientWidth = 0;

		PROPERTY( clientHeight )
		int32 m_clientHeight = 0;
	};
}
