#include "stdafx.h"
#include "ForwardRenderer.h"

#include "IAga.h"
#include "RenderView.h"
#include "Scene/Scene.h"
#include "Scene/SceneConstantBuffers.h"
#include "Viewport.h"

bool ForwardRenderer::PrepareRender( RenderViewGroup& renderViewGroup )
{
	auto rendertargetSize = renderViewGroup.Viewport( ).Size( );

	auto& depthStencil = m_renderTargets.GetDepthStencil( );
	depthStencil.UpdateBufferSize( rendertargetSize.first, rendertargetSize.second );

	IScene& scene = renderViewGroup.Scene( );
	return UpdateGPUPrimitiveInfos( *scene.GetRenderScene( ) );
}

void ForwardRenderer::Render( RenderViewGroup& renderViewGroup )
{
	auto& depthStencil = m_renderTargets.GetDepthStencil( );
	depthStencil.Clear( 1.f, 0 );

	SetRenderTarget( renderViewGroup );

	IScene& scene = renderViewGroup.Scene( );

	auto& viewConstant = scene.SceneViewConstant( );

	for ( auto& view : renderViewGroup )
	{
		ViewConstantBufferParameters viewConstantParam;
		FillViewConstantParam( viewConstantParam, view );

		viewConstant.Update( viewConstantParam );

		RenderMesh( scene, view );
	}
}

void ForwardRenderer::SetRenderTarget( RenderViewGroup& renderViewGroup )
{
	auto depthStencil = m_renderTargets.GetDepthStencil( ).Texture( );
	auto renderTarget = renderViewGroup.Viewport( ).Texture( );

	GetInterface<IAga>( )->BindRenderTargets( &renderTarget, 1, depthStencil );
}
