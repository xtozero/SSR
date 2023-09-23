#pragma once

#include "AppConfig/AppConfig.h"
#include "GuideTypes.h"

#include <string>

namespace logic
{
	class DefaultLogic : public ConfigBase<DefaultLogic, "default_engine">
	{
		GENERATE_CLASS_TYPE_INFO( DefaultLogic );

	public:
		static const char* GetDefaultWorld();

		static const float4& GetDefaultBackgroundColor();

	private:
		PROPERTY( defaultWorld );
		std::string m_defaultWorld;

		PROPERTY( defaultBackgroundColor )
		float m_defaultBackgroundColor[4];
	};
}
