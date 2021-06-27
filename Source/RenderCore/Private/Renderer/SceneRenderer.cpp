#include "stdafx.h"
#include "Renderer/SceneRenderer.h"

#include "ConstantSlotDefine.h"
#include "Mesh/StaticMeshResource.h"
#include "Material/MaterialResource.h"
#include "Proxies/PrimitiveProxy.h"
#include "Proxies/TexturedSkyProxy.h"
#include "RenderView.h"
#include "Scene/IScene.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"
#include "Scene/SceneConstantBuffers.h"

void SceneRenderer::WaitUntilRenderingIsFinish( )
{
	PrimitiveIdVertexBufferPool::GetInstance( ).DiscardAll( );
}

void SceneRenderer::RenderTexturedSky( IScene& scene )
{
	Scene* renderScene = scene.GetRenderScene( );
	if ( renderScene == nullptr )
	{
		return;
	}

	TexturedSkyProxy* proxy = renderScene->TexturedSky( );
	if ( proxy == nullptr )
	{
		return;
	}

	StaticMeshRenderData* renderData = proxy->GetRenderData( );
	MaterialResource* material = proxy->GetMaterialResource( );
	if ( ( renderData == nullptr ) || ( renderData->LODSize( ) == 0 || ( material == nullptr ) ) )
	{
		return;
	}

	StaticMeshLODResource& lodResource = renderData->LODResource( 0 );
	VertexLayoutDesc vertexLayoutDesc = renderData->VertexLayout( 0 ).Desc( );

	for ( const auto& section : lodResource.m_sections )
	{
		DrawSnapshot snapshot;
		snapshot.m_vertexStream.Bind( lodResource.m_vb, 0 );
		snapshot.m_indexBuffer = lodResource.m_ib;

		auto& pipelineState = snapshot.m_pipelineState;
		material->TakeSnapShot( snapshot );

		auto binding = snapshot.m_shaderBindings.GetSingleShaderBindings( SHADER_TYPE::VS );
		auto& viewConstant = scene.SceneViewConstant( );
		binding.AddConstantBuffer( VS_CONSTANT_BUFFER::VIEW_PROJECTION, viewConstant.Resource( ) );

		auto& graphicsInterface = GraphicsInterface( );
		pipelineState.m_shaderState.m_vertexLayout = graphicsInterface.FindOrCreate( pipelineState.m_shaderState.m_vertexShader, vertexLayoutDesc );

		pipelineState.m_depthStencilState = graphicsInterface.FindOrCreate( proxy->GetDepthStencilOption( ) );
		pipelineState.m_rasterizerState = graphicsInterface.FindOrCreate( proxy->GetRasterizerOption( ) );

		pipelineState.m_primitive = RESOURCE_PRIMITIVE::TRIANGLELIST;

		snapshot.m_indexCount = section.m_count;
		snapshot.m_startIndexLocation = section.m_startLocation;
		snapshot.m_baseVertexLocation = 0;

		auto& shaderState = pipelineState.m_shaderState;

		aga::PipelineStateInitializer initializer
		{
			shaderState.m_vertexShader.Resource( ),
			shaderState.m_pixelShader.Resource( ),
			pipelineState.m_blendState.Resource( ),
			pipelineState.m_rasterizerState.Resource( ),
			pipelineState.m_depthStencilState.Resource( ),
			shaderState.m_vertexLayout.Resource( ),
			pipelineState.m_primitive,
		};

		pipelineState.m_pso = aga::PipelineState::Create( initializer );

		CommitDrawSnapshot( snapshot );
	}
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
		aga::ShaderParameter primitiveInfo = parameter.GetParameter( "primitiveInfo" );

		auto srv = scene.GpuPrimitiveInfo( ).SRV( );
		binding.AddSRV( primitiveInfo.m_bindPoint, srv );
	}

	VertexBuffer primitiveIds = PrimitiveIdVertexBufferPool::GetInstance( ).Alloc( view.m_drawSnapshots.size( ) * sizeof( UINT ) );

	PreparePipelineStateObject( view.m_drawSnapshots );
	SortDrawSnapshots( view.m_drawSnapshots, primitiveIds );
	CommitDrawSnapshots( view.m_drawSnapshots );
}