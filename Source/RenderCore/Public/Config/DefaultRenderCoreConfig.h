#pragma once

#include "AppConfig/AppConfig.h"
#include "Reflection.h"

namespace rendercore
{
	class DefaultRenderCore final : public ConfigBase<DefaultRenderCore, "default_engine">
	{
		GENERATE_CLASS_TYPE_INFO( DefaultRenderCore );

	public:
		static bool IsTaaEnabled();

		static bool IsESMsEnabled();
		static float ESMsParamC();

		static bool IsRSMsEnabled();
		static uint32 RSMsNumSampling();
		static float RSMsMaxSamplingRadius();

		static bool UseIrradianceMapSH();

		static uint32 RenderTargetPoolMin();

		static bool IsLpvEnabled();
		static uint32 NumLpvIteration();

	private:
		PROPERTY( enableTAA )
		bool m_enableTAA = false;

		PROPERTY( enableESMs )
		bool m_enableESMs = false;

		PROPERTY( esmsParamC )
		float m_esmsParamC = 1.f;

		PROPERTY( enableRSMs )
		bool m_enableRSMs = false;

		PROPERTY( rsmsNumSampling )
		uint32 m_rsmsNumSampling = 400;

		PROPERTY( rsmsMaxSampleRadius )
		float m_rsmsMaxSampleRadius = 0.1f;

		PROPERTY( useIrradianceMapSH )
		bool m_useIrradianceMapSH = true;

		PROPERTY( renderTargetPoolMin )
		uint32 m_renderTargetPoolMin = 300;

		PROPERTY( enableLPV )
		bool m_enableLPV = false;

		PROPERTY( numLpvIteration )
		uint32 m_numLpvIteration = 8;
	};
}
