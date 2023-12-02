#pragma once

namespace rendercore
{
	class ShadowInfo;

	void BuildOrthoShadowProjectionMatrix( ShadowInfo& shadowInfo );
	void BuildPSMProjectionMatrix( ShadowInfo& shadowInfo );
	void BuildLSPSMProjectionMatrix( ShadowInfo& shadowInfo );
	void BuildPointShadowProjectionMatrix( ShadowInfo& shadowInfo );
	void CalculateSplitPositions( ShadowInfo& shadowInfo, float casterFar, float lamda = 0.5f );
}
