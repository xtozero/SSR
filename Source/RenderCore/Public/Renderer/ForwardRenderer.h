#pragma once
#include "SceneRenderer.h"

#include "SizedTypes.h"

namespace rendercore
{
	class ForwardRendererRenderTargets
	{
	public:
		void UpdateBufferSize( uint32 width, uint32 height );

		aga::Texture* GetDepthStencil();
		aga::Texture* GetLinearDepth();
		aga::Texture* GetWorldNormal();

	private:
		void AllocDepthStencil();
		void AllocLinearDepth();
		void AllocWorldNormal();

		void ReleaseAll();

		aga::RefHandle<aga::Texture> m_depthStencil;
		aga::RefHandle<aga::Texture> m_linearDepth;
		aga::RefHandle<aga::Texture> m_worldNormal;

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
