#include "PassProcessor.h"

#include "AbstractGraphicsInterface.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GraphicsApiResource.h"
#include "MaterialResource.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "VertexCollection.h"

namespace rendercore
{
	std::optional<DrawSnapshot> IPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		assert( IsInRenderThread() );

		static MaterialResource dummyMaterial;
		PassShader passShader = CollectPassShader( ( subMesh.m_material != nullptr ) ? *subMesh.m_material : dummyMaterial );

		return ProcessInternal( subMesh, passShader );
	}

	PassShader IPassProcessor::CollectPassShader( MaterialResource& material ) const
	{
		StaticShaderSwitches vsSwitches = material.GetShaderSwitches( agl::ShaderType::Vertex );
		StaticShaderSwitches gsSwitches = material.GetShaderSwitches( agl::ShaderType::Geometry );
		StaticShaderSwitches psSwitches = material.GetShaderSwitches( agl::ShaderType::Pixel );
		StaticShaderSwitches msSwitches = material.GetShaderSwitches( agl::ShaderType::Mesh );
		StaticShaderSwitches asSwitches = material.GetShaderSwitches( agl::ShaderType::Amplification );

		if ( DefaultRenderCore::IsTaaEnabled() )
		{
			vsSwitches.On( StaticName( "TAA" ), 1 );
			msSwitches.On( StaticName( "TAA" ), 1 );
		}

		if ( DefaultRenderCore::IsRSMsEnabled() )
		{
			psSwitches.On( StaticName( "EnableRSMs" ), 1 );
		}

		if ( DefaultRenderCore::UseIrradianceMapSH() )
		{
			psSwitches.On( StaticName( "UseIrradianceMapSH" ), 1 );
		}

		if ( agl::DefaultAgl::SupportsBindless() )
		{
			psSwitches.On( StaticName( "SupportsBindless" ), 1 );
		}

		PassShader passShader = {
			.m_vertexShader = material.GetVertexShader( &vsSwitches ),
			.m_geometryShader = material.GetGeometryShader( &gsSwitches ),
			.m_pixelShader = material.GetPixelShader( &psSwitches ),
			.m_meshShader = material.GetMeshShader( &msSwitches ),
			.m_amplificationShader = material.GetAmplificationShader( &asSwitches ),
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
		ShaderStates& shaderState = pipelineState.m_shaderState;

		bool bUseMeshShader = ( passShader.m_meshShader != nullptr );
		if ( bUseMeshShader )
		{
			shaderState.m_meshShader = passShader.m_meshShader;
			shaderState.m_amplificationShader = passShader.m_amplificationShader;
		}
		else if ( passShader.m_vertexShader )
		{
			shaderState.m_vertexShader = passShader.m_vertexShader;
			shaderState.m_geometryShader = passShader.m_geometryShader;
		}
		else
		{
			return {};
		}

		shaderState.m_pixelShader = passShader.m_pixelShader;

		auto initializer = CreateShaderBindingsInitializer( shaderState );
		snapshot.m_shaderBindings.Initialize( initializer );

		VertexStreamLayout vertexlayout;
		if ( subMesh.m_vertexCollection )
		{
			vertexlayout = subMesh.m_vertexCollection->VertexLayout( layoutType );

			if ( bUseMeshShader )
			{
				const agl::ShaderParameterMap& shaderParameterMapForMS = shaderState.m_meshShader->ParameterMap();
				agl::SingleShaderBindings shaderBindingsForMS = snapshot.m_shaderBindings.GetSingleShaderBindings( agl::ShaderType::Mesh );
				subMesh.m_vertexCollection->Bind( shaderParameterMapForMS, shaderBindingsForMS );

				assert( subMesh.m_meshlet != nullptr );
				assert( subMesh.m_meshletVertices != nullptr );
				assert( subMesh.m_meshletTriangles != nullptr );

				agl::ShaderParameter meshletParameterForMS = shaderParameterMapForMS.GetParameter( StaticName("Meshlets") );
				agl::ShaderParameter meshletVerticesParameterForMS = shaderParameterMapForMS.GetParameter( StaticName( "VertexIndices" ) );
				agl::ShaderParameter meshletTrianglesParamterForMS = shaderParameterMapForMS.GetParameter( StaticName( "TriangleIndices" ) );

				shaderBindingsForMS.AddSRV( meshletParameterForMS, subMesh.m_meshlet->SRV() );
				shaderBindingsForMS.AddSRV( meshletVerticesParameterForMS, subMesh.m_meshletVertices->SRV() );
				shaderBindingsForMS.AddSRV( meshletTrianglesParamterForMS, subMesh.m_meshletTriangles->SRV() );

				if ( shaderState.m_amplificationShader )
				{
					const agl::ShaderParameterMap& shaderParameterForAS = shaderState.m_amplificationShader->ParameterMap();
					agl::SingleShaderBindings shaderBindingsForAS = snapshot.m_shaderBindings.GetSingleShaderBindings( agl::ShaderType::Amplification );

					agl::ShaderParameter meshletParameterForAS = shaderParameterForAS.GetParameter( StaticName( "Meshlets" ) );

					shaderBindingsForAS.AddSRV( meshletParameterForAS, subMesh.m_meshlet->SRV() );
				}
			}
			else
			{
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
		}

		if ( subMesh.m_indexBuffer )
		{
			snapshot.m_indexBuffer = *subMesh.m_indexBuffer;
		}

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

		snapshot.m_count = bUseMeshShader ? subMesh.m_numMeshlets : subMesh.m_count;
		snapshot.m_startIndexLocation = subMesh.m_startLocation;
		snapshot.m_baseVertexLocation = subMesh.m_baseVertexLocation;

		if ( pipelineState.m_shaderState.m_vertexShader )
		{
			pipelineState.m_shaderState.m_vertexLayout = graphicsInterface.FindOrCreate( *pipelineState.m_shaderState.m_vertexShader, vertexlayout );
		}

		PreparePipelineStateObject( snapshot );

		return snapshot;
	}

	IPassProcessor* PassProcessorManager::GetPassProcessor( RenderPassType passType )
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

	void PassProcessorManager::RegisterCreateFunction( RenderPassType passType, PassProcessorCreateFunction createFunction )
	{
		int32 index = static_cast<int32>( passType );
		m_createFunctions[index] = createFunction;
	}

	PassProcessorRegister::PassProcessorRegister( RenderPassType passType, PassProcessorCreateFunction createFunction )
	{
		PassProcessorManager::RegisterCreateFunction( passType, createFunction );
	}

	std::unique_ptr<IPassProcessor> PassProcessorManager::m_passProcessors[static_cast<uint32>( RenderPassType::Count )] = {};
	PassProcessorCreateFunction PassProcessorManager::m_createFunctions[static_cast<uint32>( RenderPassType::Count )] = {};
}
