#include "Config/DefaultRenderCoreConfig.h"

namespace rendercore
{
	bool DefaultRenderCore::IsTaaEnabled()
	{
		return GetInstance().m_enableTAA;
	}
}
