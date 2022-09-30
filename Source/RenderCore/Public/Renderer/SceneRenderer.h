#pragma once

#include "DrawSnapshot.h"
#include "GraphicsApiResource.h"
#include "NameTypes.h"
#include "PassProcessor.h"
#include "RenderCoreAllocator.h"
#include "Scene/SceneConstantBuffers.h"
#include "Scene/ShadowInfo.h"
#include "TemporalAntiAliasingRendering.h"
#include "Texture.h"

#include <array>
#include <map>
#include <string>

namespace agl
{
	class GraphicsApiResource;
	class ShaderBindings;
	class ICommandList;
}

namespace rendercore
{
	class IScene;
	class RenderViewGroup;

	struct RenderView;
	struct ShaderStates;

	class IRendererRenderTargets
	{
	public:
		virtual agl::Texture* GetDepthStencil() = 0;
		virtual agl::Texture* GetLinearDepth() = 0;
		virtual agl::Texture* GetTAAHistory() = 0;
		virtual agl::Texture* GetTAAResolve() = 0;
		virtual agl::Texture* GetWorldNormal() = 0;
		virtual agl::Texture* GetVelocity() = 0;
	};

	class RenderingShaderResource
	{
	public:
		void BindResources( const ShaderStates& shaders, agl::ShaderBindings& bindings );
		void AddResource( const std::string& parameterName, agl::GraphicsApiResource* resource );
		void ClearResources();

	private:
		std::vector<Name> m_parameterNames;
		std::vector<agl::GraphicsApiResource*> m_resources;
	};

	struct RenderingOutputContext
	{
		agl::RefHandle<agl::Texture> m_renderTargets[agl::MAX_RENDER_TARGET] = {};
		agl::RefHandle<agl::Texture> m_depthStencil;

		agl::CubeArea<float> m_viewport;
		agl::RectangleArea<int32> m_scissorRects;
	};

	class SceneRenderer
	{
	public:
		virtual bool PreRender( RenderViewGroup& renderViewGroup );
		virtual void Render( RenderViewGroup& renderViewGroup ) = 0;
		virtual void PostRender( RenderViewGroup& renderViewGroup );

		virtual void RenderDefaultPass( RenderViewGroup& renderViewGroup, uint32 curView ) = 0;

		template <typename CommandList>
		void ApplyOutputContext( CommandList& commandList )
		{
			agl::RenderTargetView* renderTargets[agl::MAX_RENDER_TARGET] = {};
			agl::DepthStencilView* depthStencil = nullptr;

			for ( uint32 i = 0; i < agl::MAX_RENDER_TARGET; ++i )
			{
				if ( m_outputContext.m_renderTargets[i] )
				{
					renderTargets[i] = m_outputContext.m_renderTargets[i]->RTV();
				}
			}

			if ( m_outputContext.m_depthStencil )
			{
				depthStencil = m_outputContext.m_depthStencil->DSV();
			}

			commandList.BindRenderTargets( renderTargets, agl::MAX_RENDER_TARGET, depthStencil );
			commandList.SetViewports( 1, &m_outputContext.m_viewport );
			commandList.SetScissorRects( 1, &m_outputContext.m_scissorRects );
		}

		virtual IRendererRenderTargets& GetRenderRenderTargets() = 0;

		static void WaitUntilRenderingIsFinish();

		virtual ~SceneRenderer() = default;
	protected:
		void InitDynamicShadows( RenderViewGroup& renderViewGroup );
		void ClassifyShadowCasterAndReceiver( IScene& scene, const VectorSingleFrame<ShadowInfo*>& shadows );
		void SetupShadow();
		void AllocateShadowMaps();
		void AllocateCascadeShadowMaps( const VectorSingleFrame<ShadowInfo*>& shadows );

		void RenderShadowDepthPass();
		void RenderTexturedSky( IScene& scene );
		void RenderMesh( IScene& scene, RenderPass passType, RenderView& renderView );
		void RenderShadow();
		void RenderSkyAtmosphere( IScene& scene, RenderView& renderView );
		void RenderVolumetricCloud( IScene& scene, RenderView& renderView );
		void RenderTemporalAntiAliasing( RenderViewGroup& renderViewGroup );

		void StoreOuputContext( const RenderingOutputContext& context );

		RenderingShaderResource m_shaderResources;
		RenderingOutputContext m_outputContext;

		VectorSingleFrame<ShadowInfo> m_shadowInfos;
		using PassVisibleSnapshots = std::array<VectorSingleFrame<VisibleDrawSnapshot>, static_cast<uint32>( RenderPass::Count )>;
		VectorSingleFrame<PassVisibleSnapshots> m_passSnapshots;

		std::vector<PreviousFrameContext> m_prevFrameContext;

	private:
		TAARenderer m_taa;
	};
}
