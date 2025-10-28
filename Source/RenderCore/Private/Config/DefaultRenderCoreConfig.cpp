#include "Config/DefaultRenderCoreConfig.h"

namespace rendercore
{
	bool DefaultRenderCore::UseVSync()
	{
		return GetInstance().m_useVSync;
	}

	bool DefaultRenderCore::AllowTearing()
	{
		return GetInstance().m_allowTearing;
	}

	const float4& DefaultRenderCore::GetDefaultBackgroundColor()
	{
		return GetInstance().m_defaultBackgroundColor;
	}

	bool DefaultRenderCore::IsPIXEnabled()
	{
		return GetInstance().m_enablePIX;
	}

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

	uint32 DefaultRenderCore::GraphicsResourcePoolMin()
	{
		return GetInstance().m_graphicsResourcePoolMin;
	}

	bool DefaultRenderCore::IsLpvEnabled()
	{
		return GetInstance().m_enableLPV;
	}

	uint32 DefaultRenderCore::NumLpvIteration()
	{
		return GetInstance().m_numLpvIteration;
	}

	bool DefaultRenderCore::IsSSGIEnabled()
	{
		return GetInstance().m_enableSSGI;
	}

	SSGIConfig DefaultRenderCore::GetSSGIConfig()
	{
		const DefaultRenderCore& renderCoreConfig = GetInstance();
		
		return SSGIConfig{
			.m_thickness = renderCoreConfig.m_thicknessSSGI,
			.m_viewSpaceRadius = renderCoreConfig.m_viewSpaceRadiusSSGI,
			.m_numSlices = renderCoreConfig.m_numSlicesSSGI,
			.m_numSteps = renderCoreConfig.m_numStepsSSGI,
			.m_colorIntensity = renderCoreConfig.m_colorIntensitySSGI,
			.m_denoiseKernelRadius = renderCoreConfig.m_denoiseKernelRadiusSSGI,
		};
	}

	bool DefaultRenderCore::SupportsVisibilityRendering()
	{
		return GetInstance().m_enableVisibilityRendering && agl::DefaultAgl::SupportsBindless();
	}
}
