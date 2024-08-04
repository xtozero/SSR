#pragma once

#include "AppConfig/AppConfig.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace agl
{
	class DefaultAgl final : public ConfigBase<DefaultAgl, "default_engine">
	{
		GENERATE_CLASS_TYPE_INFO( DefaultAgl );

	public:
		static AglType GetType();
		AGL_DLL static uint32 GetBufferCount();
		AGL_DLL static bool IsSupportsBindless();
		static bool IsGpuValidationEnabled();
		
	private:
		PROPERTY( type )
		uint8 m_type = static_cast<uint8>( AglType::D3D11 );

		PROPERTY( bufferCount )
		uint32 m_bufferCount = 1;

		PROPERTY( useBindless )
		bool m_useBindless = false;

		PROPERTY( gpuValidationEnabled )
		bool m_gpuValidationEnabled = false;
	};
}
