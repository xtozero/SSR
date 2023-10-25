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

	uint32 DefaultRenderCore::RSMsNumSampling()
	{
		return GetInstance().m_rsmsNumSampling;
	}

	float DefaultRenderCore::RSMsMaxSamplingRadius()
	{
		return GetInstance().m_rsmsMaxSampleRadius;
	}

	bool DefaultRenderCore::UseIrradianceMapSH()
	{
		return GetInstance().m_useIrradianceMapSH;
	}
}
