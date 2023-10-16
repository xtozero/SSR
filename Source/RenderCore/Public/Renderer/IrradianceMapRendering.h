#pragma once

#include "GraphicsApiResource.h"
#include "Texture.h"

namespace rendercore
{
	agl::RefHandle<agl::Texture> GenerateIrradianceMap( agl::RefHandle<agl::Texture> cubeMap );
}
