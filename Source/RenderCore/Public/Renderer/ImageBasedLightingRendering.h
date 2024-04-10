#pragma once

#include "GraphicsApiResource.h"
#include "Math/Vector.h"
#include "Texture.h"

#include <array>

namespace rendercore
{
	[[nodiscard]] agl::RefHandle<agl::Texture> GenerateIrradianceMap( agl::RefHandle<agl::Texture> cubeMap );
	[[nodiscard]] std::array<Vector, 9> GenerateIrradianceMapSH( agl::RefHandle<agl::Texture> cubeMap );

	[[nodiscard]] agl::RefHandle<agl::Texture> GeneratePrefilteredSpecular( agl::RefHandle<agl::Texture> cubeMap );
}
