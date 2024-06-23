#pragma once

#include "GraphicsApiResource.h"
#include "Texture.h"

namespace rendercore
{
	class ShadowInfo;

	RefHandle<agl::Texture> GenerateExponentialShadowMaps( const ShadowInfo& shadowInfo, RefHandle<agl::Texture> shadowMap );
}