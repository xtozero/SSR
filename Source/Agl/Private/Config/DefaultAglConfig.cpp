#include "Config/DefaultAglConfig.h"

namespace agl
{
	bool DefaultAgl::UseVSync()
	{
		return GetInstance().m_useVSync;
	}

	bool DefaultAgl::AllowTearing()
	{
		return GetInstance().m_allowTearing;
	}

	AglType DefaultAgl::GetType()
	{
		return static_cast<AglType>( GetInstance().m_type );
	}

	uint32 DefaultAgl::GetBufferCount()
	{
		return GetInstance().m_bufferCount;
	}

	bool DefaultAgl::SupportsBindless()
	{
		return ( GetType() == AglType::D3D12 ) && GetInstance().m_useBindless;
	}

	bool DefaultAgl::IsGpuValidationEnabled()
	{
		return GetInstance().m_gpuValidationEnabled;
	}
}
