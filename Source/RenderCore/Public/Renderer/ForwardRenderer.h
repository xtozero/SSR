#pragma once
#include "SceneRenderer.h"

#include "SizedTypes.h"

namespace rendercore
{
	class ForwardRendererRenderTargets
	{
	public:
		void UpdateBufferSize( uint32 width, uint32 height );

		agl::Texture* GetDepthStencil();
		agl::Texture* GetLinearDepth();
		agl::Texture* GetWorldNormal();

	private:
		void AllocDepthStencil();
		void AllocLinearDepth();
		void AllocWorldNormal();

		void ReleaseAll();

		agl::RefHandle<agl::Texture> m_depthStencil;
		agl::RefHandle<agl::Texture> m_linearDepth;
		agl::RefHandle<agl::Texture> m_worldNormal;

		std::pair<uint32, uint32> m_bufferSize;
	};

	class ForwardRenderer final : public SceneRenderer
	{
	public:
		virtual bool PreRender( RenderViewGroup& renderViewGroup ) override;
		virtual void Render( RenderViewGroup& renderViewGroup ) override;
		virtual void PostRender( RenderViewGroup& renderViewGroup ) override;

		virtual void RenderDefaultPass( RenderViewGroup& renderViewGroup, uint32 curView ) override;

		void RenderDepthPass( RenderViewGroup& renderViewGroup, uint32 curView );

	private:
		void UpdateLightResource( RenderViewGroup& renderViewGroup );

		ForwardRendererRenderTargets m_renderTargets;
	};
}
