#pragma once

#include "GraphicsApiResource.h"
#include "Texture.h"

namespace rendercore
{
	class RenderGraph;
	class ShadowInfo;

	RefHandle<agl::Texture> GenerateExponentialShadowMaps( RenderGraph& renderGraph, const ShadowInfo& shadowInfo, RefHandle<agl::Texture> shadowMap );
}