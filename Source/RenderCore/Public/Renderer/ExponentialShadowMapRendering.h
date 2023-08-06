#pragma once

#include "GraphicsApiResource.h"
#include "Texture.h"

namespace rendercore
{
	class ShadowInfo;

	agl::RefHandle<agl::Texture> GenerateExponentialShadowMaps( const ShadowInfo& shadowInfo, agl::RefHandle<agl::Texture> shadowMap );
}