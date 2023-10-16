#pragma once

#include "GlobalShaders.h"

namespace rendercore
{
	class FullScreenQuadVS : public GlobalShaderCommon<VertexShader, FullScreenQuadVS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	public:
		FullScreenQuadVS() = default;
	};
}
