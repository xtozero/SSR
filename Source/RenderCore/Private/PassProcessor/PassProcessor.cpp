#include "PassProcessor.h"

#include "AbstractGraphicsInterface.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GraphicsApiResource.h"
#include "MaterialResource.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "VertexCollection.h"

namespace rendercore
{
	PassShader IPassProcessor::CollectPassShader( MaterialResource& material ) const
	{
		StaticShaderSwitches vsSwitches = material.GetShaderSwitches( agl::ShaderType::VS );
		StaticShaderSwitches gsSwitches = material.GetShaderSwitches( agl::ShaderType::GS );
		StaticShaderSwitches psSwitches = material.GetShaderSwitches( agl::ShaderType::PS );
		StaticShaderSwitches asSwitches = material.GetShaderSwitches( agl::ShaderType::AS );
		StaticShaderSwitches msSwitches = material.GetShaderSwitches( agl::ShaderType::MS );

		if ( DefaultRenderCore::IsTaaEnabled() )
		{
			vsSwitches.On( Name( "TAA" ), 1 );
		}

		if ( DefaultRenderCore::IsRSMsEnabled() )
		{
			psSwitches.On( Name( "EnableRSMs" ), 1 );
		}

		if ( DefaultRenderCore::UseIrradianceMapSH() )
		{
			psSwitches.On( Name( "UseIrradianceMapSH" ), 1 );
		}

		if ( agl::DefaultAgl::IsSupportsBindless() )
		{
			psSwitches.On( Name( "SupportsBindless" ), 1 );
		}

		PassShader passShader = {
			.m_vertexShader = material.GetVertexShader( &vsSwitches ),
			.m_geometryShader = material.GetGeometryShader( &gsSwitches ),
			.m_pixelShader = material.GetPixelShader( &psSwitches ),
			.m_amplificationShader = material.GetAmplificationShader( &asSwitches ),
			.m_meshShader = material.GetMeshShader( &msSwitches ),
		};

		return passShader;
	}

	std::optional<DrawSnapshot> IPassProcessor::BuildDrawSnapshot( 
		const PrimitiveSubMesh& subMesh, 
		const PassShader& passShader, 
		const PassRenderOption& passRenderOption, 
		VertexStreamLayoutType layoutType, 
		bool useAutoInstancing )
	{
		DrawSnapshot snapshot;
		snapshot.m_primitiveIdSlot = -1;

		GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;

		if ( passShader.m_meshShader )
		{
			pipelineState.m_shaderState.m_amplificationShader = passShader.m_amplificationShader;
			pipelineState.m_shaderState.m_meshShader = passShader.m_meshShader;
		}
		else if ( passShader.m_vertexShader )
		{
			pipelineState.m_shaderState.m_vertexShader = passShader.m_vertexShader;
			pipelineState.m_shaderState.m_geometryShader = passShader.m_geometryShader;
		}
		else
		{
			return {};
		}

		pipelineState.m_shaderState.m_pixelShader = passShader.m_pixelShader;

		VertexStreamLayout vertexlayout;
		if ( subMesh.m_vertexCollection )
		{
			vertexlayout = subMesh.m_vertexCollection->VertexLayout( layoutType );

			if ( useAutoInstancing )
			{
				uint32 primitiveIdSlot = vertexlayout.Size();
				vertexlayout.AddLayout( "PRIMITIVEID", 0,
					agl::ResourceFormat::R32_UINT,
					primitiveIdSlot,
					true,
					1,
					-1 );

				snapshot.m_primitiveIdSlot = primitiveIdSlot;
			}

			subMesh.m_vertexCollection->Bind( snapshot.m_vertexStream, layoutType );
		}

		if ( subMesh.m_indexBuffer )
		{
			snapshot.m_indexBuffer = *subMesh.m_indexBuffer;
		}

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

		pipelineState.m_primitive = passRenderOption.m_primitive;

		snapshot.m_count = subMesh.m_count;
		snapshot.m_startIndexLocation = subMesh.m_startLocation;
		snapshot.m_baseVertexLocation = subMesh.m_baseVertexLocation;

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
