#pragma once

#include "GlobalShaders.h"
#include "GraphicsApiResource.h"
#include "Texture.h"

namespace rendercore
{
	class FullScreenQuadVS final : public GlobalShaderCommon<VertexShader, FullScreenQuadVS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	public:
		FullScreenQuadVS() = default;
	};

	class DefaultGraphicsResources final
	{
	public:
		static DefaultGraphicsResources& GetInstance()
		{
			static DefaultGraphicsResources defaultGraphicsResources;
			return defaultGraphicsResources;
		}

		void BootUp();
		void Shutdown();
	};

	extern agl::RefHandle<agl::Texture> BlackTexture;
	extern agl::RefHandle<agl::Texture> WhiteTexture;

	extern agl::RefHandle<agl::Texture> BlackCubeTexture;
	extern agl::RefHandle<agl::Texture> WhiteCubeTexture;

	extern agl::RefHandle<agl::Texture> BRDFLookUpTexture;
}
