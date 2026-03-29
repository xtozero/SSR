#pragma once

#include "GlobalShaders.h"
#include "IndexBuffer.h"
#include "Texture.h"

#include <atomic>

namespace rendercore
{
	class RenderGraph;
	class RenderGraphTexture;

	class FullScreenQuadVS final : public GlobalShaderBase<VertexShader, FullScreenQuadVS>
	{
		using GlobalShaderBase::GlobalShaderBase;
	};

	class DefaultAS final : public GlobalShaderBase<AmplificationShader, DefaultAS>
	{
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

	RENDERCORE_DLL RefHandle<agl::Texture> CreateBRDFLookUpTexture();

	struct DenoisePassParams
	{
		RenderGraphTexture* m_prevImage = nullptr;
		RenderGraphTexture* m_image = nullptr;
		RenderGraphTexture* m_prevViewSpaceDistance = nullptr;
		RenderGraphTexture* m_viewSpaceDistance = nullptr;
		RenderGraphTexture* m_velocity = nullptr;

		int32 m_kernelRadius = 0;
		Vector2 m_screenSize = Vector2::ZeroVector;

		bool IsValid() const;
	};

	RefHandle<agl::Texture> AddDenoisePass( RenderGraph& renderGraph, const DenoisePassParams& params );

	extern RefHandle<agl::Texture> BlackTexture;
	extern RefHandle<agl::Texture> WhiteTexture;

	extern RefHandle<agl::Texture> BlackCubeTexture;
	extern RefHandle<agl::Texture> WhiteCubeTexture;

	extern RefHandle<agl::Texture> BRDFLookUpTexture;

	extern IndexBuffer OcclusionQueryIndexBuffer;
}
