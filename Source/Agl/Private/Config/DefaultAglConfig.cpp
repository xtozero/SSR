#include "Config/DefaultAglConfig.h"

namespace agl
{
	AglType DefaultAgl::GetType()
	{
		return static_cast<AglType>( GetInstance().m_type );
	}

	uint32 DefaultAgl::GetBufferCount()
	{
		return GetInstance().m_bufferCount;
	}

	bool DefaultAgl::IsSupportsBindless()
	{
		return ( GetType() == AglType::D3D12 ) && GetInstance().m_useBindless;
	}
}
