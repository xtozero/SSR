#pragma once

#include "GlobalShaders.h"

namespace rendercore
{
	class VertexShader;

	class FullScreenQuadVS : public GlobalShaderCommon<VertexShader, FullScreenQuadVS>
	{
	public:
		FullScreenQuadVS( const StaticShaderSwitches& switches ) : GlobalShaderCommon<VertexShader, FullScreenQuadVS>( switches ) {}
		FullScreenQuadVS() = default;
	};
}
