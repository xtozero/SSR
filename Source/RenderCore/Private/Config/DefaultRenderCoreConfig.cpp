#include "Config/DefaultRenderCoreConfig.h"

namespace rendercore
{
	bool DefaultRenderCore::IsTaaEnabled()
	{
		return GetInstance().m_enableTAA;
	}

	bool DefaultRenderCore::IsESMsEnabled()
	{
		return GetInstance().m_enableESMs;
	}

	float DefaultRenderCore::ESMsParamC()
	{
		return GetInstance().m_esmsParamC;
	}

	bool DefaultRenderCore::IsRSMsEnabled()
	{
		return GetInstance().m_enableRSMs;
	}
}
