#include "stdafx.h"
#include "PassProcessor.h"

#include "AbstractGraphicsInterface.h"
#include "MaterialResource.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "VertexCollection.h"

std::optional<DrawSnapshot> IPassProcessor::BuildDrawSnapshot( const PrimitiveSubMesh& subMesh, const PassShader& passShader, const PassRenderOption& passRenderOption, VertexStreamLayoutType layoutType )
{
	VertexStreamLayout vertexlayout = subMesh.m_vertexCollection->VertexLayout( layoutType );

	uint32 primitiveIdSlot = vertexlayout.Size( );
	vertexlayout.AddLayout( "PRIMITIVEID", 0,
							RESOURCE_FORMAT::R32_UINT,
							primitiveIdSlot,
							true,
							1,
							-1 );

	DrawSnapshot snapshot;
	subMesh.m_vertexCollection->Bind( snapshot.m_vertexStream, layoutType );
	snapshot.m_primitiveIdSlot = primitiveIdSlot;
	snapshot.m_indexBuffer = *subMesh.m_indexBuffer;

	GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
	pipelineState.m_shaderState.m_vertexShader = passShader.m_vertexShader;
	pipelineState.m_shaderState.m_pixelShader = passShader.m_pixelShader;

	auto materialResource = subMesh.m_material;
	if ( materialResource )
	{
		materialResource->TakeSnapshot( snapshot, pipelineState.m_shaderState );
	}

	auto& graphicsInterface = GraphicsInterface( );
	if ( passRenderOption.m_blendOption )
	{
		pipelineState.m_blendState = graphicsInterface.FindOrCreate( *passRenderOption.m_blendOption );
	}

	if ( passRenderOption.m_depthStencilOption )
	{
		pipelineState.m_depthStencilState = graphicsInterface.FindOrCreate( *passRenderOption.m_depthStencilOption );
	}

	if ( passRenderOption.m_rasterizerOption )
	{
		pipelineState.m_rasterizerState = graphicsInterface.FindOrCreate( *passRenderOption.m_rasterizerOption );
	}

	pipelineState.m_primitive = RESOURCE_PRIMITIVE::TRIANGLELIST;

	snapshot.m_indexCount = subMesh.m_count;
	snapshot.m_startIndexLocation = subMesh.m_startLocation;
	snapshot.m_baseVertexLocation = 0;

	if ( pipelineState.m_shaderState.m_vertexShader )
	{
		pipelineState.m_shaderState.m_vertexLayout = graphicsInterface.FindOrCreate( *pipelineState.m_shaderState.m_vertexShader, vertexlayout );
	}

	PreparePipelineStateObject( snapshot );

	return snapshot;
}

IPassProcessor* PassProcessorManager::GetPassProcessor( RenderPass passType )
{
	int32 index = static_cast<int32>( passType );
	auto& passProcessor = m_passProcessors[index];
	auto createFunction = m_createFunctions[index];
	if ( passProcessor == nullptr && createFunction )
	{
		passProcessor.reset( m_createFunctions[index]( ) );
	}

	return passProcessor.get( );
}

void PassProcessorManager::RegisterCreateFunction( RenderPass passType, PassProcessorCreateFunction createFunction )
{
	int32 index = static_cast<int32>( passType );
	m_createFunctions[index] = createFunction;
}

PassProcessorRegister::PassProcessorRegister( RenderPass passType, PassProcessorCreateFunction createFunction )
{
	PassProcessorManager::RegisterCreateFunction( passType, createFunction );
}

std::unique_ptr<IPassProcessor> PassProcessorManager::m_passProcessors[RenderPass::Count] = {};
PassProcessorCreateFunction PassProcessorManager::m_createFunctions[RenderPass::Count] = {};
