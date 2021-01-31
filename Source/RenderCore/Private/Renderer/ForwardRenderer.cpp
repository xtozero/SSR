#include "stdafx.h"
#include "ForwardRenderer.h"

#include "IAga.h"
#include "RenderView.h"
#include "GraphicsResource/Viewport.h"

void ForwardRenderer::PrepareRender( RenderViewGroup& renderViewGroup )
{
	auto rendertargetSize = renderViewGroup.Viewport( ).Size( );

	auto& depthStencil = m_renderTargets.GetDepthStencil( );
	depthStencil.UpdateBufferSize( rendertargetSize.first, rendertargetSize.second );
}

void ForwardRenderer::Render( RenderViewGroup& renderViewGroup )
{
	auto& depthStencil = m_renderTargets.GetDepthStencil( );
	depthStencil.Clear( 0.f, 0 );

	SetRenderTarget( renderViewGroup );
}

void ForwardRenderer::SetRenderTarget( RenderViewGroup& renderViewGroup )
{
	auto depthStencil = m_renderTargets.GetDepthStencil( ).Texture( );
	auto renderTarget = renderViewGroup.Viewport( ).Texture( );

	GetInterface<IAga>( )->BindRenderTargets( &renderTarget, 1, depthStencil );
}
