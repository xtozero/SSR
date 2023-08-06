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
		static bool IsESMsEnabled();
		static float ESMsParamC();

	private:
		PROPERTY( enableTAA )
		bool m_enableTAA = false;

		PROPERTY( enableESMs )
		bool m_enableESMs = false;

		PROPERTY( esmsParamC )
		float m_esmsParamC = 1.f;
	};
}
