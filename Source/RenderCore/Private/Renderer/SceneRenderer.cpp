#include "stdafx.h"
#include "Renderer/SceneRenderer.h"

#include "ConstantSlotDefine.h"
#include "Proxies/PrimitiveProxy.h"
#include "RenderView.h"
#include "Scene/IScene.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/SceneConstantBuffers.h"

void SceneRenderer::WaitUntilRenderingIsFinish( )
{
	PrimitiveIdVertexBufferPool::GetInstance( ).DiscardAll( );
}

void SceneRenderer::RenderMesh( IScene& scene, RenderView& view )
{
	const auto& primitives = scene.Primitives( );
	if ( primitives.size( ) == 0 )
	{
		return;
	}

	auto& viewConstant = scene.SceneViewConstant( );

	// Create DrawSnapshot
	for ( auto primitive : primitives )
	{
		PrimitiveProxy* proxy = primitive->m_sceneProxy;
		proxy->TakeSnapshot( view.m_drawSnapshots );

		auto& snapshot = view.m_drawSnapshots.back( );
		auto binding = snapshot.m_shaderBindings.GetSingleShaderBindings( SHADER_TYPE::VS );
		binding.AddConstantBuffer( VS_CONSTANT_BUFFER::VIEW_PROJECTION, viewConstant.Resource( ) );

		auto parameter = snapshot.m_pipelineState.m_shaderState.m_vertexShader.ParameterMap( );
		ShaderParameter primitiveInfo = parameter.GetParameter( "primitiveInfo" );

		auto srv = scene.GpuPrimitiveInfo( ).SRV( );
		binding.AddSRV( primitiveInfo.m_bindPoint, srv );
	}

	VertexBuffer primitiveIds = PrimitiveIdVertexBufferPool::GetInstance( ).Alloc( view.m_drawSnapshots.size( ) * sizeof( UINT ) );

	PreparePipelineStateObject( view.m_drawSnapshots );
	SortDrawSnapshot( view.m_drawSnapshots, primitiveIds );
	CommitDrawSnapshot( view.m_drawSnapshots, primitiveIds );
}
