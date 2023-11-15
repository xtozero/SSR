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

		agl::RefHandle<agl::Texture> m_depthStencil;
		agl::RefHandle<agl::Texture> m_linearDepth;
		agl::RefHandle<agl::Texture> m_taaHistory;
		agl::RefHandle<agl::Texture> m_taaResolve;
		agl::RefHandle<agl::Texture> m_worldNormal;
		agl::RefHandle<agl::Texture> m_velocity;

		std::pair<uint32, uint32> m_bufferSize;
	};

	class ForwardRenderer final : public SceneRenderer
	{
	public:
		virtual bool PreRender( RenderViewGroup& renderViewGroup ) override;
		virtual void Render( RenderViewGroup& renderViewGroup ) override;
		virtual void PostRender( RenderViewGroup& renderViewGroup ) override;

		virtual void RenderDefaultPass( RenderViewGroup& renderViewGroup, uint32 curView ) override;

		virtual IRendererRenderTargets& GetRenderRenderTargets() override;

		void RenderDepthPass( RenderViewGroup& renderViewGroup, uint32 curView );

	private:
		void UpdateLightResource( IScene& scene );

		ForwardRendererRenderTargets m_renderTargets;
	};
}
