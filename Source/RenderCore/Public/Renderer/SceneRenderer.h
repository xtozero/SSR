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
#include "SSGIRendering.h"
#include "TemporalAntiAliasingRendering.h"
#include "Texture.h"
#include "VisibilityRendering.h"

#include <array>
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

		RenderFrameArray<VisibleDrawSnapshot>* m_drawSnapshots = nullptr;
		RenderFrameArray<bool> m_visibilityMap;

		DebugOverlayData m_debugOverlayData;
	};

	class IRendererRenderTargets
	{
	public:
		virtual agl::Texture* GetSceneColor() = 0;
		virtual agl::Texture* GetDepthStencil() = 0;
		virtual agl::Texture* GetPrevViewSpaceDistance() = 0;
		virtual agl::Texture* GetViewSpaceDistance() = 0;
		virtual agl::Texture* GetTAAHistory() = 0;
		virtual agl::Texture* GetTAAResolve() = 0;
		virtual agl::Texture* GetWorldNormal() = 0;
		virtual agl::Texture* GetVelocity() = 0;
		virtual agl::Texture* GetVisibility() = 0;

		virtual ~IRendererRenderTargets() = default;
	};

	class RendererResourceCollection
	{
	public:
		void Clear();
		
		RefHandle<agl::Texture> m_indirectIllumination;
		RefHandle<agl::Texture> m_ssgi;
	};

	class ResourceBinder final
	{
	public:
		void Bind( const ShaderStates& shaders, agl::ShaderBindings& outBindings ) const;
		void Bind( const ComputeShader* shader, agl::ShaderBindings& outBindings ) const;

		void Add( const Name& parameterName, agl::GraphicsApiResource* resource );
		void Add( const ShaderArguments* collection );

		void Clear();

	private:
		void Bind( const ShaderBase* (&shaders)[agl::MAX_SHADER_TYPE<uint32>], agl::ShaderBindings& outBindings ) const;

		std::vector<Name> m_parameterNames;
		std::vector<agl::GraphicsApiResource*> m_resources;

		std::vector<const ShaderArguments*> m_argumentsList;
	};

	enum class RasterOutputLoadAction : uint8
	{
		NoAction = 0,
		Clear,
	};

	class RasterOutput final
	{
	public:
		void SetRenderTarget( int32 index, RenderGraphTexture* renderTarget, RasterOutputLoadAction loadAction = RasterOutputLoadAction::NoAction );
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

		virtual void RenderDefaultPass( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex ) = 0;

		virtual IRendererRenderTargets& GetRenderTargets() = 0;

		virtual ~SceneRenderer() = default;

	protected:
		void InitDynamicShadows( RenderViewGroup& renderViewGroup );
		void ClassifyViewDependentShadowCasterAndReceiver( IScene& scene, const RenderFrameArray<ShadowInfo*>& shadows );
		void ClassifyViewIndependentShadowCasterAndReceiver( const RenderFrameArray<ShadowInfo*>& shadows );
		void SetupShadow();
		void AllocateShadowMaps();
		void AllocateCascadeShadowMaps( const RenderFrameArray<ShadowInfo*>& shadows );
		void AllocatePointShadowMaps( const RenderFrameArray<ShadowInfo*>& shadows );

		RenderFrameArray<VisibleDrawSnapshot>* GatherSortedDrawSnapshots( IScene& scene, RenderPassType passType, uint32 viewIndex );

		VisibilityPassData BuildVisibilityPassData( IScene& scene, uint32 viewIndex );

		void RenderShadowDepthPass( RenderGraph& renderGraph );
		void RenderTexturedSky( RenderGraph& renderGraph, IScene& scene, const RasterOutput& rasterOutput );
		void RenderShadow( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );
		void RenderSkyAtmosphere( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex );
		void RenderVolumetricCloud( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );
		void RenderVolumetricFog( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );
		void RenderTemporalAntiAliasing( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );
		void RenderIndirectIllumination( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex );
		virtual void RenderScreenSpaceIndirectIllumination( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex );
		void RenderDebugOverlay( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex );
		void DoRenderHitProxy( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );
		void ResolveSceneColor( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );

		void CalcVisibility( RenderViewGroup& renderViewGroup );

		ForwardLightingResource m_forwardLighting;

		ResourceBinder m_resourceBinder;
		RendererResourceCollection m_resourceCollection;

		RenderFrameArray<ShadowInfo> m_shadowInfos;
		using PassVisibleDrawSnapshots = std::array<RenderFrameArray<VisibleDrawSnapshot>, static_cast<uint32>( RenderPassType::Count )>;
		RenderFrameArray<PassVisibleDrawSnapshots> m_drawSnapshotsByView;
		RenderFrameArray<OcclusionRenderData> m_occlusionRenderData;
		RenderFrameArray<DrawSnapshot> m_curFrameDrawSnapshots;

		RenderFrameArray<ShadingSnapshot> m_curFrameShadingSnapshots;

		std::vector<RenderViewInfo, InlineAllocator<RenderViewInfo, 1>> m_viewInfo;
		std::vector<PreviousFrameContext> m_prevFrameContext;

		GlobalDynamicVertexBuffer m_dynamicVertexBuffer;

	private:
		void ResetFrameData();

		LightPropagationVolume m_lpv;
		RSMsRenderer m_rsms;
		SSGIRenderer m_ssgi;
		TAARenderer m_taa;
	};

	void AddSingleDrawPass( CommandList& commandList, DrawSnapshot& snapshot );

	PrimitiveIdVertexBufferPool& GetPrimitiveIdPool();
}
