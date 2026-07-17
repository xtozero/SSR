#include "Scene/VolumetricCloudSceneInfo.h"

#include "CommandList.h"
#include "ComputePipelineState.h"
#include "GlobalShaders.h"
#include "RenderGraph.h"
#include "ShaderParameterMap.h"
#include "ShaderParameterUtils.h"

namespace rendercore
{
	class PerlinWorleyCS final : public GlobalShaderBase<ComputeShader, PerlinWorleyCS>
	{
		DECLARE_SHADER_PARAM( NoiseTex );
	};

	class WorleyCS final : public GlobalShaderBase<ComputeShader, WorleyCS>
	{
		DECLARE_SHADER_PARAM( NoiseTex );
	};

	class WeatherMapCS final : public GlobalShaderBase<ComputeShader, WeatherMapCS>
	{
		DECLARE_SHADER_PARAM( WeatherTex );
	};

	REGISTER_GLOBAL_SHADER( PerlinWorleyCS, "Cloud/CS_PerlinWorley.fx","main" );
	REGISTER_GLOBAL_SHADER( WorleyCS, "Cloud/CS_Worley.fx", "main" );
	REGISTER_GLOBAL_SHADER( WeatherMapCS, "Cloud/CS_WeatherMap.fx", "main" );

	void VolumetricCloudSceneInfo::CreateRenderData()
	{
		if ( m_needCreateRenderData )
		{
			m_shaderArguments = VolumetricCloudRenderParameters::CreateShaderArguments();

			RenderGraph renderGraph;

			SetupCloudTexture( renderGraph );
			GenerateWeatherMap( renderGraph );

			renderGraph.Execute();

			RenderGraph::Commit();

			GetInterface<agl::IAgl>()->WaitGPU();

			m_needCreateRenderData = false;
		}
	}

	VolumetricCloudSceneInfo::VolumetricCloudSceneInfo( VolumetricCloudProxy* proxy )
		: m_cloudProxy( proxy )
	{
	}

	void VolumetricCloudSceneInfo::SetupCloudTexture( RenderGraph& renderGraph )
	{
		m_baseCloudShape = CreateCloudTexture( 128 );
		auto rgBaseCloudShape = renderGraph.RegisterExternalResource( m_baseCloudShape.Get() );

		BEGIN_RG_RESOURCE_STRUCT( BaseCloudShapePassResource )
			DECLARE_RG_TEXTURE_UAV( baseCloudShape )
		END_RG_RESOURCE_STRUCT();

		BaseCloudShapePassResource baseCloudShapePassResource = {
			.m_baseCloudShape = rgBaseCloudShape
		};

		renderGraph.AddPass(
			baseCloudShapePassResource,
			[baseCloudShapePassResource]( ComputeCommandList& commandList )
			{
				PerlinWorleyCS perlinWorleyCS;
				auto threadGroupCount = static_cast<uint32>( std::ceilf( 128 / 8.f ) );

				RefHandle<agl::ComputePipelineState> perlinWorleyPSO = PrepareComputePipelineState( perlinWorleyCS );
				commandList.BindPipelineState( perlinWorleyPSO.Get() );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( perlinWorleyCS );
				auto baseCloudShape = baseCloudShapePassResource.m_baseCloudShape->Get();
				BindResource( shaderBindings, perlinWorleyCS.NoiseTex(), baseCloudShape );

				commandList.BindShaderResources( shaderBindings );

				commandList.Dispatch( threadGroupCount, threadGroupCount, threadGroupCount );
			} );

		m_detailCloudShape = CreateCloudTexture( 32 );
		auto rgDetailCloudShape = renderGraph.RegisterExternalResource( m_detailCloudShape.Get() );

		BEGIN_RG_RESOURCE_STRUCT( DetailCloudShapePassResource )
			DECLARE_RG_TEXTURE_UAV( detailCloudShape )
		END_RG_RESOURCE_STRUCT();

		DetailCloudShapePassResource detailCloudShapePassResource = {
			.m_detailCloudShape = rgDetailCloudShape
		};

		renderGraph.AddPass(
			detailCloudShapePassResource,
			[detailCloudShapePassResource]( ComputeCommandList& commandList )
			{
				WorleyCS worleyCS;
				auto threadGroupCount = static_cast<uint32>( std::ceilf( 32 / 8.f ) );

				RefHandle<agl::ComputePipelineState> worleyPSO = PrepareComputePipelineState( worleyCS );
				commandList.BindPipelineState( worleyPSO.Get() );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( worleyCS );
				auto detailCloudShape = detailCloudShapePassResource.m_detailCloudShape->Get();
				BindResource( shaderBindings, worleyCS.NoiseTex(), detailCloudShape );

				commandList.BindShaderResources( shaderBindings );

				commandList.Dispatch( threadGroupCount, threadGroupCount, threadGroupCount );
			} );
	}

	void VolumetricCloudSceneInfo::GenerateWeatherMap( RenderGraph& renderGraph )
	{
		agl::TextureDesc desc = {
			.m_width = 1024,
			.m_height = 1024,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::None,
		};

		m_weatherMap = agl::Texture::Create( desc, "VolumetricCloud.Weather" );

		auto rgWeatherMap = renderGraph.RegisterExternalResource( m_weatherMap.Get() );

		BEGIN_RG_RESOURCE_STRUCT( WeatherMapPassResource )
			DECLARE_RG_TEXTURE_UAV( weatherMap )
		END_RG_RESOURCE_STRUCT();

		WeatherMapPassResource passResource = {
			.m_weatherMap = rgWeatherMap
		};

		renderGraph.AddPass(
			passResource,
			[passResource]( ComputeCommandList& commandList )
			{
				WeatherMapCS weatherMapCS;

				auto threadGroupCount = static_cast<uint32>( std::ceilf( 1024.f / 8.f ) );

				RefHandle<agl::ComputePipelineState> weatherMapPSO = PrepareComputePipelineState( weatherMapCS );
				commandList.BindPipelineState( weatherMapPSO.Get() );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( weatherMapCS );
				BindResource( shaderBindings, weatherMapCS.WeatherTex(), passResource.m_weatherMap->Get() );

				commandList.BindShaderResources( shaderBindings );

				commandList.Dispatch( threadGroupCount, threadGroupCount );
			} );
	}

	RefHandle<agl::Texture> VolumetricCloudSceneInfo::CreateCloudTexture( uint32 texSize )
	{
		agl::TextureDesc desc = {
			.m_width = texSize,
			.m_height = texSize,
			.m_depth = texSize,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::Texture3D
		};

		RefHandle<agl::Texture> cloudTex = agl::Texture::Create( desc, "VolumetricCloud.Cloud" );

		return cloudTex;
	}
}
