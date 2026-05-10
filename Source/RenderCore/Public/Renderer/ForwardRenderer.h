#pragma once
#include "SceneRenderer.h"

#include "SizedTypes.h"

namespace rendercore
{
	class ForwardRendererRenderTargets final : public IRendererRenderTargets
	{
	public:
		void UpdateBufferSize( uint32 width, uint32 height );

		void Tick();

		virtual agl::Texture* GetSceneColor() override;
		virtual agl::Texture* GetDepthStencil() override;
		virtual agl::Texture* GetViewSpaceDistance() override;
		virtual agl::Texture* GetPrevViewSpaceDistance() override;
		virtual agl::Texture* GetWorldNormal() override;
		virtual agl::Texture* GetVelocity() override;
		virtual agl::Texture* GetVisibility() override;

	private:
		void AllocSceneColor();
		void AllocDepthStencil();
		void AllocViewSpaceDistance();
		void AllocWorldNormal();
		void AllocVelocity();
		void AllocVisibility();

		void ReleaseAll();

		RefHandle<agl::Texture> m_sceneColor;
		RefHandle<agl::Texture> m_depthStencil;
		RefHandle<agl::Texture> m_linearDepth[2]; // 0 : current frame, 1 : previous frame
		RefHandle<agl::Texture> m_worldNormal;
		RefHandle<agl::Texture> m_velocity;
		RefHandle<agl::Texture> m_visibility;

		std::pair<uint32, uint32> m_bufferSize;
	};

	class ForwardRenderer final : public SceneRenderer
	{
	public:
		virtual void PreRender( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup ) override;
		virtual void Render( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup ) override;
		virtual void RenderHitProxy( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup ) override;

		virtual IRendererRenderTargets& GetRenderTargets() override;

	protected:
		virtual void RenderScreenSpaceIndirectIllumination( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex ) override;

	private:
		void RenderDepthPass( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex );
		void RenderDefaultPass( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex );
		void RenderOcclusionTest( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex );
		void RenderDefaultPassWithVisibilityBuffer( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup, uint32 viewIndex );
		void RenderAmbientOcclusion( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup );

		void UpdateLightResource( IScene& scene );
		void ClearRenderTargets( RenderGraph& renderGraph );

		ForwardRendererRenderTargets m_renderTargets;
	};
}
