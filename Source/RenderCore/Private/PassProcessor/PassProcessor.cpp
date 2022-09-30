#include "stdafx.h"
#include "PassProcessor.h"

#include "AbstractGraphicsInterface.h"
#include "GraphicsApiResource.h"
#include "MaterialResource.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "VertexCollection.h"

namespace rendercore
{
	PassShader IPassProcessor::CollectPassShader( MaterialResource& material ) const
	{
		StaticShaderSwitches switches = material.GetShaderSwitches( agl::ShaderType::VS );
		switches.On( Name( "TAA" ), 1 );

		PassShader passShader{
			material.GetVertexShader( &switches ),
			nullptr,
			material.GetPixelShader()
		};

		return passShader;
	}

	std::optional<DrawSnapshot> IPassProcessor::BuildDrawSnapshot( const PrimitiveSubMesh& subMesh, const PassShader& passShader, const PassRenderOption& passRenderOption, VertexStreamLayoutType layoutType )
	{
		DrawSnapshot snapshot;

		VertexStreamLayout vertexlayout;
		if ( subMesh.m_vertexCollection )
		{
			vertexlayout = subMesh.m_vertexCollection->VertexLayout( layoutType );

			uint32 primitiveIdSlot = vertexlayout.Size();
			vertexlayout.AddLayout( "PRIMITIVEID", 0,
				agl::ResourceFormat::R32_UINT,
				primitiveIdSlot,
				true,
				1,
				-1 );

			subMesh.m_vertexCollection->Bind( snapshot.m_vertexStream, layoutType );
			snapshot.m_primitiveIdSlot = primitiveIdSlot;
		}
		else
		{
			snapshot.m_primitiveIdSlot = -1;
		}

		if ( subMesh.m_indexBuffer )
		{
			snapshot.m_indexBuffer = *subMesh.m_indexBuffer;
		}

		GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
		pipelineState.m_shaderState.m_vertexShader = passShader.m_vertexShader;
		pipelineState.m_shaderState.m_geometryShader = passShader.m_geometryShader;
		pipelineState.m_shaderState.m_pixelShader = passShader.m_pixelShader;

		auto initializer = CreateShaderBindingsInitializer( pipelineState.m_shaderState );
		snapshot.m_shaderBindings.Initialize( initializer );

		auto materialResource = subMesh.m_material;
		if ( materialResource )
		{
			materialResource->TakeSnapshot( snapshot );
		}

		auto& graphicsInterface = GraphicsInterface();
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

		pipelineState.m_primitive = agl::ResourcePrimitive::Trianglelist;

		snapshot.m_count = subMesh.m_count;
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
			passProcessor.reset( m_createFunctions[index]() );
		}

		return passProcessor.get();
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

	std::unique_ptr<IPassProcessor> PassProcessorManager::m_passProcessors[static_cast<uint32>( RenderPass::Count )] = {};
	PassProcessorCreateFunction PassProcessorManager::m_createFunctions[static_cast<uint32>( RenderPass::Count )] = {};
}
