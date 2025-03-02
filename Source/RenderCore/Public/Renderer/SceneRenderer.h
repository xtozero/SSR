#pragma once

#include "AreaTypes.h"
#include "DebugOverlay.h"
#include "DrawSnapshot.h"
#include "ForwardLighting.h"
#include "GraphicsApiResource.h"
#include "LightPropagationVolume.h"
#include "NameTypes.h"
#include "OcclusionRendering.h"
#include "PassProcessor.h"
#include "ReflectiveShadowMapRendering.h"
#include "RenderGraphResource.h"
#include "RenderView.h"
#include "Scene/SceneConstantBuffers.h"
#include "Scene/ShadowInfo.h"
#include "TemporalAntiAliasingRendering.h"
#include "Texture.h"

#include <array>
#include <deque>
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
	class RenderGraph;
	class RenderGraphPass;
	class RenderViewGroup;
	class ShaderArguments;

	struct ShaderStates;

	struct RenderViewInfo : public RenderView
	{
		Matrix m_viewMatrix = Matrix::Identity;
		Matrix m_projMatrix = Matrix::Identity;
		Matrix m_viewProjMatrix = Matrix::Identity;

		RenderThreadFrameData<VisibleDrawSnapshot>* m_snapshots = nullptr;
		RenderThreadFrameData<bool> m_visibilityMap;

		DebugOverlayData m_debugOverlayData;
	};

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

	struct RendererResourceCollection
	{
		RefHandle<agl::Texture> m_indirectIllumination;
	};

	class ResourceBinder final
	{
	public:
		void Bind( const ShaderStates& shaders, agl::ShaderBindings& bindings ) const;

		void Add( const std::string& parameterName, agl::GraphicsApiResource* resource );
		void Add( const ShaderArguments* collection );

		void Clear();

	private:
		std::vector<Name> m_parameterNames;
		std::vector<agl::GraphicsApiResource*> m_resources;

		std::vector<const ShaderArguments*> m_argumentsList;
	};

	enum class RasterOutputLoadAction
	{
		NoAction = 0,
		Clear,
	};

	class RasterOutput final
	{
	public:
		void SetRenderTarget( int32 i, RenderGraphTexture* renderTarget, RasterOutputLoadAction loadAction = RasterOutputLoadAction::NoAction );
		void SetDepthStencil( RenderGraphTexture* depthStencil, bool readOnly = false, RasterOutputLoadAction loadAction = RasterOutputLoadAction::NoAction );

		void SetViewport( int32 left, int32 top, int32 right, int32 bottom, int32 front, int32 back );
		void SetViewport( int32 width, int32 height );

		void SetScissorRect( int32 left, int32 top, int32 right, int32 bottom );
		void SetScissorRect( int32 width, int32 height );

		void Bind( CommandList& commandList ) const;

	private:
		friend RenderGraphPass;

		struct RasterOutputInfo
		{
			RenderGraphTexture* m_texture = nullptr;
			RasterOutputLoadAction m_loadAction = RasterOutputLoadAction::NoAction;
		};
		RasterOutputInfo m_renderTargets[agl::MAX_RENDER_TARGET] = {};

		RasterOutputInfo m_depthStencil;
		bool m_depthStencilReadOnly = false;

		CubeArea<float> m_viewport = {};
		RectangleArea<int32> m_scissorRect = {};
	};

	class SceneRenderer
	{
	public:
		virtual void PreRender( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );
		virtual void Render( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup ) = 0;
		virtual void RenderHitProxy( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup ) = 0;

		virtual void RenderDefaultPass( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 curView ) = 0;

		virtual IRendererRenderTargets& GetRenderRenderTargets() = 0;

		virtual ~SceneRenderer() = default;

	protected:
		void InitDynamicShadows( RenderViewGroup& renderViewGroup );
		void ClassifyViewDependentShadowCasterAndReceiver( IScene& scene, const RenderThreadFrameData<ShadowInfo*>& shadows );
		void ClassifyViewIndependentShadowCasterAndReceiver( const RenderThreadFrameData<ShadowInfo*>& shadows );
		void SetupShadow();
		void AllocateShadowMaps();
		void AllocateCascadeShadowMaps( const RenderThreadFrameData<ShadowInfo*>& shadows );
		void AllocatePointShadowMaps( const RenderThreadFrameData<ShadowInfo*>& shadows );

		RenderThreadFrameData<VisibleDrawSnapshot>* GatherDrawsnapshots( IScene& scene, RenderPassType passType, uint32 viewIndex, std::deque<DrawSnapshot>& outSnapshotStorage );

		void RenderShadowDepthPass( RenderGraph& renderGraph );
		void RenderTexturedSky( RenderGraph& renderGraph, IScene& scene, const RasterOutput& rasterOutput );
		void RenderShadow( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );
		void RenderSkyAtmosphere( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex );
		void RenderVolumetricCloud( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );
		void RenderVolumetricFog( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );
		void RenderTemporalAntiAliasing( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );
		void RenderIndirectIllumination( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );
		void DoRenderHitProxy( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );

		void CalcVisibility( RenderViewGroup& renderViewGroup );

		ForwardLightingResource m_forwardLighting;

		ResourceBinder m_resourceBinder;
		RendererResourceCollection m_resourceCollection;

		RenderThreadFrameData<ShadowInfo> m_shadowInfos;
		using PassVisibleSnapshots = std::array<RenderThreadFrameData<VisibleDrawSnapshot>, static_cast<uint32>( RenderPassType::Count )>;
		RenderThreadFrameData<PassVisibleSnapshots> m_passSnapshots;
		RenderThreadFrameData<OcclusionRenderData> m_occlusionRenderData;

		std::vector<RenderViewInfo, InlineAllocator<RenderViewInfo, 1>> m_viewInfo;
		std::vector<PreviousFrameContext> m_prevFrameContext;

		GlobalDynamicVertexBuffer m_dynamicVertexBuffer;

	private:
		TAARenderer m_taa;
		RSMsRenderer m_rsms;
		LightPropagationVolume m_lpv;
	};

	void AddSingleDrawPass( CommandList& commandList, DrawSnapshot& snapshot );

	PrimitiveIdVertexBufferPool& GetPrimitiveIdPool();
}
