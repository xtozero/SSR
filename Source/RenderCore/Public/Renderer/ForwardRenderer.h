#pragma once
#include "SceneRenderer.h"

#include "SizedTypes.h"

class ForwardRendererRenderTargets
{
public:
	void UpdateBufferSize( uint32 width, uint32 height );

	aga::Texture* GetDepthStencil( );
	aga::Texture* GetLinearDepth( );

private:
	void AllocDepthStencil( );
	void AllocLinearDepth( );

	void ReleaseAll( );

	RefHandle<aga::Texture> m_depthStencil;
	RefHandle<aga::Texture> m_linearDepth;

	std::pair<uint32, uint32> m_bufferSize;
};

class ForwardRenderer final : public SceneRenderer
{
public:
	virtual bool PreRender( RenderViewGroup& renderViewGroup ) override;
	virtual void Render( RenderViewGroup& renderViewGroup ) override;
	virtual void PostRender( RenderViewGroup& renderViewGroup ) override;

	virtual void RenderDepthPass( RenderViewGroup& renderViewGroup, uint32 curView ) override;

	virtual void RenderDefaultPass( RenderViewGroup& renderViewGroup, uint32 curView ) override;

private:
	void UpdateLightResource( RenderViewGroup& renderViewGroup );

	ForwardRendererRenderTargets m_renderTargets;
};