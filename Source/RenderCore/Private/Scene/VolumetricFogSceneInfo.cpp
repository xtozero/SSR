#include "Scene/VolumetricFogSceneInfo.h"

namespace rendercore
{
	void VolumetricFogSceneInfo::CreateRenderData()
	{
	}

	VolumetricFogSceneInfo::VolumetricFogSceneInfo( VolumetricFogProxy* proxy )
		: m_volumetricFogProxy( proxy )
	{
	}
}
