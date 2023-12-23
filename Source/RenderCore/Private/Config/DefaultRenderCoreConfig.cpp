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
		return GetInstance().m_enableRSMs || GetInstance().m_enableLPV;
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

	uint32 DefaultRenderCore::RenderTargetPoolMin()
	{
		return GetInstance().m_renderTargetPoolMin;
	}

	bool DefaultRenderCore::IsLpvEnabled()
	{
		return GetInstance().m_enableLPV;
	}

	uint32 DefaultRenderCore::NumLpvIteration()
	{
		return GetInstance().m_numLpvIteration;
	}
}
