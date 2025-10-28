#include "ImageBasedLightingRendering.h"

#include "AbstractGraphicsInterface.h"
#include "CommandList.h"
#include "ComputePipelineState.h"
#include "GlobalShaders.h"
#include "GraphicsResourcePool.h"
#include "PassProcessor.h"
#include "RenderGraph.h"
#include "RenderOption.h"
#include "ResourceBarrierUtils.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "SceneRenderer.h"
#include "ShaderParameterUtils.h"
#include "StaticState.h"
#include "VertexCollection.h"

#include <cassert>

namespace rendercore
{
	class DrawIrradianceMapVS final : public GlobalShaderCommon<VertexShader, DrawIrradianceMapVS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class DrawIrradianceMapGS final : public GlobalShaderCommon<GeometryShader, DrawIrradianceMapGS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class DrawIrradianceMapPS final : public GlobalShaderCommon<PixelShader, DrawIrradianceMapPS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class IrradianceMapShCS final : public GlobalShaderCommon<ComputeShader, IrradianceMapShCS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	private:
		DEFINE_SHADER_PARAM( CubeMap );
		DEFINE_SHADER_PARAM( LinearSampler );
		DEFINE_SHADER_PARAM( Coeffs );
	};

	class PrefilteredSpecularCS final : public GlobalShaderCommon<ComputeShader, PrefilteredSpecularCS>
	{
	private:
		DEFINE_SHADER_PARAM( Roughness );
		DEFINE_SHADER_PARAM( EnvMap );
		DEFINE_SHADER_PARAM( EnvMapSampler );
		DEFINE_SHADER_PARAM( Prefiltered );
	};

	REGISTER_GLOBAL_SHADER( DrawIrradianceMapVS, "./Assets/Shaders/IndirectLighting/IrradianceMap/VS_DrawIrradianceMap.asset" );
	REGISTER_GLOBAL_SHADER( DrawIrradianceMapGS, "./Assets/Shaders/IndirectLighting/IrradianceMap/GS_DrawIrradianceMap.asset" );
	REGISTER_GLOBAL_SHADER( DrawIrradianceMapPS, "./Assets/Shaders/IndirectLighting/IrradianceMap/PS_DrawIrradianceMap.asset" );
	REGISTER_GLOBAL_SHADER( IrradianceMapShCS, "./Assets/Shaders/IndirectLighting/IrradianceMap/CS_IrradianceMapSH.asset" );
	REGISTER_GLOBAL_SHADER( PrefilteredSpecularCS, "./Assets/Shaders/PhysicallyBased/CS_PrefilteredSpecular.asset" );

	class IrradianceMapGenerateProcessor final : public IPassProcessor
	{
	protected:
		virtual std::optional<DrawSnapshot> ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader ) override;
		virtual PassShader CollectPassShader( MaterialResource& material ) const override;
	};

	std::optional<DrawSnapshot> IrradianceMapGenerateProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
		DepthStencilOption RSMsDrawPassDepthOption;
		RSMsDrawPassDepthOption.m_depth.m_enable = false;
		RSMsDrawPassDepthOption.m_depth.m_writeDepth = false;

		PassRenderOption passRenderOption = {
			.m_primitive = agl::ResourcePrimitive::Pointlist,
			.m_depthStencilOption = &RSMsDrawPassDepthOption
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::PositionOnly );
	}

	PassShader IrradianceMapGenerateProcessor::CollectPassShader( [[maybe_unused]] MaterialResource& material ) const
	{
		PassShader passShader = {
			.m_vertexShader = DrawIrradianceMapVS(),
			.m_geometryShader = DrawIrradianceMapGS(),
			.m_pixelShader = DrawIrradianceMapPS()
		};

		return passShader;
	}

	RefHandle<agl::Texture> GenerateIrradianceMap( RefHandle<agl::Texture> cubeMap )
	{
		assert( IsInRenderThread() );
		assert( cubeMap.Get() != nullptr );
		assert( cubeMap->IsCubeMap() && ( cubeMap->SRV() != nullptr ) );

		RenderGraph renderGraph;
		auto rgCubeMap = renderGraph.RegisterExternalResource( cubeMap.Get() );

		BEGIN_RG_RESOURCE_STRUCT( IrrdianceMapPassResource )
			DECLARE_RG_TEXTURE_PIXEL_SRV( cubeMap )
		END_RG_RESOURCE_STRUCT();

		IrrdianceMapPassResource passResource = {
			.m_cubeMap = rgCubeMap
		};

		agl::TextureTrait trait = cubeMap->GetTrait();
		trait.m_width = trait.m_height = 32;
		trait.m_format = agl::ResourceFormat::R8G8B8A8_UNORM;
		trait.m_bindType |= agl::ResourceBindType::RenderTarget;
		trait.m_clearValue = agl::ResourceClearValue{
			.m_format = trait.m_format,
			.m_color = { 0, 0, 0, 1 }
		};

		auto irradianceMap = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "DiffuseIrradianceMap" );
		auto rgIrrdianceMap = renderGraph.RegisterExternalResource( irradianceMap.Get() );

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgIrrdianceMap, RasterOutputLoadAction::Clear );
		rasterOutput.SetViewport( trait.m_width, trait.m_height );
		rasterOutput.SetScissorRect( trait.m_width, trait.m_height );

		renderGraph.AddPass(
			passResource,
			rasterOutput,
			[passResource]( CommandList& commandList )
			{
				IrradianceMapGenerateProcessor irradianceMapGenerateProcessor;

				PrimitiveSubMesh meshInfo;
				meshInfo.m_count = 6;

				auto result = irradianceMapGenerateProcessor.Process( meshInfo );
				if ( result.has_value() )
				{
					auto linearSampler = StaticSamplerState<>::Get();
					ResourceBinder resourceBinder;
					resourceBinder.Add( StaticName( "CubeMap" ), passResource.m_cubeMap->SRV() );
					resourceBinder.Add( StaticName( "LinearSampler" ), linearSampler.Resource() );

					DrawSnapshot& snapshot = *result;
					GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
					resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

					AddSingleDrawPass( commandList, snapshot );
				}
			}
		);
		
		renderGraph.Execute();

		RenderGraph::Commit();

		return irradianceMap;
	}

	std::array<Vector, 9> GenerateIrradianceMapSH( RefHandle<agl::Texture> cubeMap )
	{
		assert( IsInRenderThread() );
		assert( cubeMap.Get() != nullptr );
		assert( cubeMap->IsCubeMap() && ( cubeMap->SRV() != nullptr ) );

		RenderGraph renderGraph;
		auto rgCubeMap = renderGraph.RegisterExternalResource( cubeMap.Get() );

		agl::BufferTrait coeffTrait = {
			.m_stride = sizeof( Vector ),
			.m_count = 9,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::BufferStructured,
			.m_format = agl::ResourceFormat::Unknown
		};
		auto rgCoeffBuffer = renderGraph.CreateBuffer( coeffTrait, "IrradianceMap.Coefficient" );

		BEGIN_RG_RESOURCE_STRUCT( SHPassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( cubeMap )
			DECLARE_RG_BUFFER_UAV( coeffs )
		END_RG_RESOURCE_STRUCT();

		SHPassResource shPassResource = {
			.m_cubeMap = rgCubeMap,
			.m_coeffs = rgCoeffBuffer
		};

		renderGraph.AddPass( 
			shPassResource,
			[shPassResource]( ComputeCommandList& commandList )
			{
				IrradianceMapShCS irradianceMapShCS;
				RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( irradianceMapShCS );

				commandList.BindPipelineState( pso.Get() );

				auto linearSampler = StaticSamplerState<>::Get();

				agl::ShaderBindings shaderBindings = CreateShaderBindings( irradianceMapShCS );
				BindResource( shaderBindings, irradianceMapShCS.CubeMap(), shPassResource.m_cubeMap->Get() );
				BindResource( shaderBindings, irradianceMapShCS.LinearSampler(), linearSampler );
				BindResource( shaderBindings, irradianceMapShCS.Coeffs(), shPassResource.m_coeffs->Get() );

				commandList.BindShaderResources( shaderBindings );

				commandList.Dispatch( 1, 1 );
			} );

		agl::BufferTrait readBackTrait = {
			.m_stride = sizeof( Vector ),
			.m_count = 9,
			.m_access = agl::ResourceAccess::Download,
			.m_bindType = agl::ResourceBindType::None,
			.m_miscFlag = agl::ResourceMisc::None,
			.m_format = agl::ResourceFormat::Unknown
		};

		auto readBackBuffer = agl::Buffer::Create( readBackTrait, "IrradianceMap.ReadBack" );
		readBackBuffer->Init();

		auto rgReadBackBuffer = renderGraph.RegisterExternalResource( readBackBuffer.Get() );

		BEGIN_RG_RESOURCE_STRUCT( CopySHPassResource )
			DECLARE_RG_BUFFER_COPY_DEST ( readBack )
			DECLARE_RG_BUFFER_COPY_SOURCE( coeffs )
		END_RG_RESOURCE_STRUCT();

		CopySHPassResource copySHPassResource = {
			.m_readBack = rgReadBackBuffer,
			.m_coeffs = rgCoeffBuffer
		};

		renderGraph.AddPass(
			copySHPassResource,
			[copySHPassResource]( CopyCommandList& commandList )
			{
				commandList.CopyResource( copySHPassResource.m_readBack->Get(), copySHPassResource.m_coeffs->Get(), false );
			}
		);

		renderGraph.Execute();

		RenderGraph::Commit();

		GetInterface<agl::IAgl>()->WaitGPU();

		auto data = GraphicsInterface().Lock<float>( readBackBuffer.Get(), agl::ResourceLockFlag::Read );

		std::array<Vector, 9> coeffs;
		std::memcpy( coeffs.data(), data, sizeof( Vector ) * 9 );

		GraphicsInterface().UnLock( readBackBuffer.Get() );

		return coeffs;
	}

	RefHandle<agl::Texture> GeneratePrefilteredSpecular( RefHandle<agl::Texture> cubeMap )
	{
		assert( IsInRenderThread() );
		assert( cubeMap.Get() != nullptr );
		assert( cubeMap->IsCubeMap() && ( cubeMap->SRV() != nullptr ) );

		auto CountMips = 
			[]( uint32 width, uint32 height )
			{
				uint32 mipLevels = 1;

				while ( width > 1 || height > 1 )
				{
					if ( width > 1 )
					{
						width >>= 1;
					}

					if ( height > 1 )
					{
						height >>= 1;
					}

					++mipLevels;
				}

				return mipLevels;
			};

		const agl::TextureTrait& cubeMapTrait = cubeMap->GetTrait();

		constexpr uint32 MaxResolution = 512;
		uint32 width = std::min( cubeMapTrait.m_width, MaxResolution );
		uint32 height = std::min( cubeMapTrait.m_height, MaxResolution );

		agl::TextureTrait trait = {
			.m_width = width,
			.m_height = height,
			.m_depth = cubeMapTrait.m_depth,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = CountMips( width, height ),
			.m_format = agl::ResourceFormat::R16G16B16A16_FLOAT,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::TextureCube
		};

		auto prefiltered = agl::Texture::Create( trait, "PrefilteredSpecular" );
		prefiltered->Init();

		RenderGraph renderGraph;
		auto rgCubeMap = renderGraph.RegisterExternalResource( cubeMap.Get() );
		auto rgPrefiltered = renderGraph.RegisterExternalResource( prefiltered.Get() );

		BEGIN_RG_RESOURCE_STRUCT( PrefilteredPassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( cubeMap )
			DECLARE_RG_TEXTURE_UAV( prefiltered )
		END_RG_RESOURCE_STRUCT();

		PrefilteredPassResource passResource = {
			.m_cubeMap = rgCubeMap,
			.m_prefiltered = rgPrefiltered
		};

		renderGraph.AddPass(
			passResource,
			[passResource, width, height, mipLevels = trait.m_mipLevels]( ComputeCommandList& commandList )
			{
				agl::Texture* cubeMap = passResource.m_cubeMap->Get();
				agl::Texture* prefiltered = passResource.m_prefiltered->Get();
				uint32 curWidth = width;
				uint32 curHeight = height;

				PrefilteredSpecularCS prefilteredSpecularCS;
				RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( prefilteredSpecularCS );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( prefilteredSpecularCS );
				BindResource( shaderBindings, prefilteredSpecularCS.EnvMap(), cubeMap );

				auto linearSampler = StaticSamplerState<>::Get();
				BindResource( shaderBindings, prefilteredSpecularCS.EnvMapSampler(), linearSampler );

				for ( uint32 mipSlice = 0; mipSlice < mipLevels; ++mipSlice )
				{
					BindResource( shaderBindings, prefilteredSpecularCS.Prefiltered(), prefiltered, mipSlice );
					SetShaderValue( commandList, prefilteredSpecularCS.Roughness(), static_cast<float>( mipSlice ) / mipLevels );

					commandList.BindPipelineState( pso.Get() );
					commandList.BindShaderResources( shaderBindings );

					uint32 numGroupX = CalcAlignment<uint32>( curWidth, 8 ) / 8;
					uint32 numGroupY = CalcAlignment<uint32>( curHeight, 8 ) / 8;
					commandList.Dispatch( numGroupX, numGroupY, 6 );

					if ( curWidth > 1 )
					{
						curWidth >>= 1;
					}

					if ( curHeight > 1 )
					{
						curHeight >>= 1;
					}
				}

				commandList.AddTransition( Transition( *cubeMap, agl::ResourceState::PixelShaderResource ) );
				commandList.AddTransition( Transition( *prefiltered, agl::ResourceState::PixelShaderResource ) );
			}
		);

		renderGraph.Execute();

		RenderGraph::Commit();

		return prefiltered;
	}
}
