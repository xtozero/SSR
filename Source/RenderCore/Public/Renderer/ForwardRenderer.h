#pragma once
#include "SceneRenderer.h"

#include "GraphicsResource/DepthStencil.h"

class ForwardRenderer final : public SceneRenderer
{
public:
	virtual bool PreRender( RenderViewGroup& renderViewGroup ) override;
	virtual void Render( RenderViewGroup& renderViewGroup ) override;
	virtual void PostRender( RenderViewGroup& renderViewGroup ) override;

	virtual void SetRenderTarget( aga::ICommandList& commandList, RenderViewGroup& renderViewGroup ) override;

private:
	void UpdateLightResource( RenderViewGroup& renderViewGroup );

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