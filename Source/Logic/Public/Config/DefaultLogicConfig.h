#pragma once

#include "AppConfig/AppConfig.h"
#include "GuideTypes.h"

#include <filesystem>
#include <string>

namespace logic
{
	class DefaultLogic : public ConfigBase<DefaultLogic, "default_engine">
	{
		GENERATE_CLASS_TYPE_INFO( DefaultLogic );

	public:
		static const char* GetDefaultWorld();
		static void SetDefaultWorld( const std::filesystem::path& worldPath );

		static const float4& GetDefaultBackgroundColor();

		static const bool IsSaveLastWorldAsDefault();

	private:
		PROPERTY( defaultWorld );
		std::string m_defaultWorld;

		PROPERTY( defaultBackgroundColor )
		float m_defaultBackgroundColor[4] = {};

		PROPERTY( saveLastWorldAsDefault )
		bool m_saveLastWorldAsDefault = false;
	};
}
