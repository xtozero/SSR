#pragma once
#include "SceneRenderer.h"

#include "GraphicsResource/DepthStencil.h"

class ForwardRenderer final : public SceneRenderer
{
public:
	virtual bool PrepareRender( RenderViewGroup& renderViewGroup ) override;
	virtual void Render( RenderViewGroup& renderViewGroup ) override;

private:
	void SetRenderTarget( RenderViewGroup& renderViewGroup );

	class ForwardRendererRenderTargets
	{
	public:
		rendercore::DepthStencil& GetDepthStencil( )
		{
			return m_depthStencil;
		}

	private:
		rendercore::DepthStencil m_depthStencil;
	} m_renderTargets;
};