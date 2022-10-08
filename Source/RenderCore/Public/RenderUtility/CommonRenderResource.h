#pragma once

#include "GlobalShaders.h"

namespace rendercore
{
	class VertexShader;

	class FullScreenQuadVS : public GlobalShaderCommon<VertexShader, FullScreenQuadVS>
	{};
}
