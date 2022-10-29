#pragma once

#include "AppConfig/AppConfig.h"
#include "Reflection.h"

namespace rendercore
{
	class DefaultRenderCore : public ConfigBase<DefaultRenderCore, "default_engine">
	{
		GENERATE_CLASS_TYPE_INFO( DefaultRenderCore );

	public:
		static bool IsTaaEnabled();

	private:
		PROPERTY( enableTAA )
		bool m_enableTAA = false;
	};
}
