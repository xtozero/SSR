#pragma once

#include "AreaTypes.h"
#include "DrawSnapshot.h"
#include "ForwardLighting.h"
#include "GraphicsApiResource.h"
#include "LightPropagationVolume.h"
#include "NameTypes.h"
#include "PassProcessor.h"
#include "ReflectiveShadowMapRendering.h"
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
	class ICommandListBase;
	class ShaderBindings;
}

namespace rendercore
{
	class IScene;
	class RenderViewGroup;
	class ShaderArguments;

	struct RenderView;
	struct ShaderStates;

	class IRendererRenderTargets
	{
	public:
		virtual agl::Texture* GetDepthStencil() = 0;
		virtual agl::Texture* GetViewSpaceDistance() = 0;
		virtual agl::Texture* GetTAAHistory() = 0;
		virtual agl::Texture* GetTAAResolve() = 0;
		virtual agl::Texture* GetWorldNormal() = 0;
		virtual agl::Texture* GetVelocity() = 0;
	};

	class RenderingShaderResource final
	{
	public:
		void BindResources( const ShaderStates& shaders, agl::ShaderBindings& bindings );

		void AddResource( const std::string& parameterName, agl::GraphicsApiResource* resource );
		void AddResource( const ShaderArguments* collection );

		void ClearResources();

	private:
		std::vector<Name> m_parameterNames;
		std::vector<agl::GraphicsApiResource*> m_resources;

		std::vector<const ShaderArguments*> m_argumentsList;
	};

	struct RenderingOutputContext final
	{
		RefHandle<agl::Texture> m_renderTargets[agl::MAX_RENDER_TARGET] = {};
		RefHandle<agl::Texture> m_depthStencil;

		CubeArea<float> m_viewport = {};
		RectangleArea<int32> m_scissorRects = {};
	};

	class SceneRenderer
	{
	public:
		virtual void PreRender( RenderViewGroup& renderViewGroup );
		virtual void Render( RenderViewGroup& renderViewGroup ) = 0;
		virtual void RenderHitProxy( RenderViewGroup& renderViewGroup ) = 0;
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

		void WaitUntilRenderingIsFinish();

		virtual ~SceneRenderer() = default;

	protected:
		void InitDynamicShadows( RenderViewGroup& renderViewGroup );
		void ClassifyViewDependentShadowCasterAndReceiver( IScene& scene, const RenderThreadFrameData<ShadowInfo*>& shadows );
		void ClassifyViewIndependentShadowCasterAndReceiver( const RenderThreadFrameData<ShadowInfo*>& shadows );
		void SetupShadow();
		void AllocateShadowMaps();
		void AllocateCascadeShadowMaps( const RenderThreadFrameData<ShadowInfo*>& shadows );
		void AllocatePointShadowMaps( const RenderThreadFrameData<ShadowInfo*>& shadows );

		void RenderShadowDepthPass();
		void RenderTexturedSky( IScene& scene );
		void RenderMesh( IScene& scene, RenderPass passType, RenderView& renderView );
		void RenderShadow();
		void RenderSkyAtmosphere( IScene& scene, RenderView& renderView );
		void RenderVolumetricCloud( IScene& scene, RenderView& renderView );
		void RenderVolumetricFog( IScene& scene, RenderView& renderView );
		void RenderTemporalAntiAliasing( RenderViewGroup& renderViewGroup );
		void RenderIndirectIllumination( RenderViewGroup& renderViewGroup );
		void DoRenderHitProxy( RenderViewGroup& renderViewGroup );

		void StoreOuputContext( const RenderingOutputContext& context );

		ForwardLightingResource m_forwardLighting;

		RenderingShaderResource m_shaderResources;
		RenderingOutputContext m_outputContext;

		RenderThreadFrameData<ShadowInfo> m_shadowInfos;
		using PassVisibleSnapshots = std::array<RenderThreadFrameData<VisibleDrawSnapshot>, static_cast<uint32>( RenderPass::Count )>;
		RenderThreadFrameData<PassVisibleSnapshots> m_passSnapshots;

		std::vector<PreviousFrameContext> m_prevFrameContext;

	private:
		TAARenderer m_taa;
		RSMsRenderer m_rsms;
		LightPropagationVolume m_lpv;
	};

	void AddSingleDrawPass( DrawSnapshot& snapshot );

	PrimitiveIdVertexBufferPool& GetPrimitiveIdPool();
}
