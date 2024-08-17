#pragma once
#include "SceneRenderer.h"

#include "SizedTypes.h"

namespace rendercore
{
	class ForwardRendererRenderTargets final : public IRendererRenderTargets
	{
	public:
		void UpdateBufferSize( uint32 width, uint32 height );

		virtual agl::Texture* GetDepthStencil() override;
		virtual agl::Texture* GetViewSpaceDistance() override;
		virtual agl::Texture* GetTAAHistory() override;
		virtual agl::Texture* GetTAAResolve() override;
		virtual agl::Texture* GetWorldNormal() override;
		virtual agl::Texture* GetVelocity() override;

	private:
		void AllocDepthStencil();
		void AllocViewSpaceDistance();
		void AllocTAARenderTargets();
		void AllocWorldNormal();
		void AllocVelocity();

		void ReleaseAll();

		RefHandle<agl::Texture> m_depthStencil;
		RefHandle<agl::Texture> m_linearDepth;
		RefHandle<agl::Texture> m_taaHistory;
		RefHandle<agl::Texture> m_taaResolve;
		RefHandle<agl::Texture> m_worldNormal;
		RefHandle<agl::Texture> m_velocity;

		std::pair<uint32, uint32> m_bufferSize;
	};

	class ForwardRenderer final : public SceneRenderer
	{
	public:
		virtual void PreRender( RenderViewGroup& renderViewGroup ) override;
		virtual void Render( RenderViewGroup& renderViewGroup ) override;
		virtual void RenderHitProxy( RenderViewGroup& renderViewGroup ) override;

		virtual void RenderDefaultPass( RenderViewGroup& renderViewGroup, uint32 curView ) override;

		virtual IRendererRenderTargets& GetRenderRenderTargets() override;

		void RenderDepthPass( RenderViewGroup& renderViewGroup, uint32 curView );
		void RenderOcclusionTest( RenderViewGroup& renderViewGroup, uint32 viewIndex );

	private:
		void UpdateLightResource( IScene& scene );

		ForwardRendererRenderTargets m_renderTargets;
	};
}
