#pragma once

#include "GraphicsApiResource.h"
#include "Math/Vector.h"
#include "Texture.h"

#include <array>

namespace rendercore
{
	[[nodiscard]] RefHandle<agl::Texture> GenerateIrradianceMap( RefHandle<agl::Texture> cubeMap );
	[[nodiscard]] std::array<Vector, 9> GenerateIrradianceMapSH( RefHandle<agl::Texture> cubeMap );

	[[nodiscard]] RefHandle<agl::Texture> GeneratePrefilteredSpecular( RefHandle<agl::Texture> cubeMap );
}
