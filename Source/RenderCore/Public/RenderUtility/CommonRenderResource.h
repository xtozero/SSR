#pragma once

#include "GlobalShaders.h"
#include "Texture.h"

#include <atomic>

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

		bool IsReady() const;

	private:
		std::atomic<int32> m_numPending = 0;
	};

	RENDERCORE_DLL agl::RefHandle<agl::Texture> CreateBRDFLookUpTexture();

	extern agl::RefHandle<agl::Texture> BlackTexture;
	extern agl::RefHandle<agl::Texture> WhiteTexture;

	extern agl::RefHandle<agl::Texture> BlackCubeTexture;
	extern agl::RefHandle<agl::Texture> WhiteCubeTexture;

	extern agl::RefHandle<agl::Texture> BRDFLookUpTexture;
}
