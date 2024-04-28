#include "ImageBasedLightingRendering.h"

#include "AbstractGraphicsInterface.h"
#include "CommandList.h"
#include "ComputePipelineState.h"
#include "GlobalShaders.h"
#include "PassProcessor.h"
#include "RenderOption.h"
#include "RenderTargetPool.h"
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
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
	};

	std::optional<DrawSnapshot> IrradianceMapGenerateProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		PassShader passShader = {
			.m_vertexShader = DrawIrradianceMapVS(),	
			.m_geometryShader = DrawIrradianceMapGS(),
			.m_pixelShader = DrawIrradianceMapPS()
		};

		DepthStencilOption RSMsDrawPassDepthOption;
		RSMsDrawPassDepthOption.m_depth.m_enable = false;
		RSMsDrawPassDepthOption.m_depth.m_writeDepth = false;

		PassRenderOption passRenderOption = {
			.m_primitive = agl::ResourcePrimitive::Pointlist,
			.m_depthStencilOption = &RSMsDrawPassDepthOption
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::PositionOnly );
	}

	agl::RefHandle<agl::Texture> GenerateIrradianceMap( agl::RefHandle<agl::Texture> cubeMap )
	{
		assert( IsInRenderThread() );
		assert( cubeMap.Get() != nullptr );
		assert( cubeMap->IsCubeMap() && ( cubeMap->SRV() != nullptr ) );

		agl::TextureTrait trait = cubeMap->GetTrait();
		trait.m_width = trait.m_height = 32;
		trait.m_format = agl::ResourceFormat::R8G8B8A8_UNORM_SRGB;
		trait.m_bindType |= agl::ResourceBindType::RenderTarget;

		auto irradianceMap = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "DiffuseIrradianceMap" );
		EnqueueRenderTask(
			[irradianceMap]()
			{
				irradianceMap->Init();
			} );

		auto commandList = GetCommandList();

		commandList.AddTransition( Transition( *irradianceMap.Get(), agl::ResourceState::RenderTarget ) );

		{
			agl::RenderTargetView* rtv = irradianceMap->RTV();

			commandList.BindRenderTargets( &rtv, 1, nullptr );

			CubeArea<float> viewport = {
				.m_left = 0.f,
				.m_top = 0.f,
				.m_front = 0.f,
				.m_right = static_cast<float>( trait.m_width ),
				.m_bottom = static_cast<float>( trait.m_height ),
				.m_back = 1.f
			};
			commandList.SetViewports( 1, &viewport );

			RectangleArea<int32> scissorRect = {
				.m_left = 0L,
				.m_top = 0L,
				.m_right = static_cast<int32>( trait.m_width ),
				.m_bottom = static_cast<int32>( trait.m_height )
			};
			commandList.SetScissorRects( 1, &scissorRect );

			IrradianceMapGenerateProcessor irradianceMapGenerateProcessor;

			PrimitiveSubMesh meshInfo;
			meshInfo.m_count = 6;

			auto result = irradianceMapGenerateProcessor.Process( meshInfo );
			if ( result.has_value() )
			{
				auto linearSampler = StaticSamplerState<>::Get();
				RenderingShaderResource shaderResources;
				shaderResources.AddResource( "CubeMap", cubeMap->SRV() );
				shaderResources.AddResource( "LinearSampler", linearSampler.Resource() );

				DrawSnapshot& snapshot = *result;
				GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
				shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

				AddSingleDrawPass( snapshot );
			}
		}

		commandList.AddTransition( Transition( *irradianceMap.Get(), agl::ResourceState::PixelShaderResource ) );

		commandList.Commit();

		return irradianceMap;
	}

	std::array<Vector, 9> GenerateIrradianceMapSH( agl::RefHandle<agl::Texture> cubeMap )
	{
		assert( IsInRenderThread() );
		assert( cubeMap.Get() != nullptr );
		assert( cubeMap->IsCubeMap() && ( cubeMap->SRV() != nullptr ) );

		agl::BufferTrait coeffTrait = {
			.m_stride = sizeof( Vector ),
			.m_count = 9,
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::BufferStructured,
			.m_format = agl::ResourceFormat::Unknown
		};

		auto coeffBuffer = agl::Buffer::Create( coeffTrait, "IrradianceMap.Coefficient" );
		EnqueueRenderTask(
			[coeffBuffer]()
			{
				coeffBuffer->Init();
			}
		);

		IrradianceMapShCS irradianceMapShCS;
		agl::RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( irradianceMapShCS );

		auto commandList = GetCommandList();
		commandList.BindPipelineState( pso );

		auto linearSampler = StaticSamplerState<>::Get();

		agl::ShaderBindings shaderBindings = CreateShaderBindings( irradianceMapShCS );
		BindResource( shaderBindings, irradianceMapShCS.CubeMap(), cubeMap );
		BindResource( shaderBindings, irradianceMapShCS.LinearSampler(), linearSampler );
		BindResource( shaderBindings, irradianceMapShCS.Coeffs(), coeffBuffer );

		commandList.BindShaderResources( shaderBindings );

		commandList.Dispatch( 1, 1 );
		commandList.Commit();

		agl::BufferTrait readBackTrait = {
			.m_stride = sizeof( Vector ),
			.m_count = 9,
			.m_access = agl::ResourceAccessFlag::Download,
			.m_bindType = agl::ResourceBindType::None,
			.m_miscFlag = agl::ResourceMisc::None,
			.m_format = agl::ResourceFormat::Unknown
		};

		auto readBackBuffer = agl::Buffer::Create( readBackTrait, "IrradianceMap.ReadBack" );
		EnqueueRenderTask(
			[readBackBuffer]()
			{
				readBackBuffer->Init();
			}
		);

		GetInterface<agl::IAgl>()->WaitGPU();
		commandList.CopyResource( readBackBuffer, coeffBuffer );

		auto data = static_cast<float*>( GraphicsInterface().Lock( readBackBuffer, agl::ResourceLockFlag::Read ).m_data );

		std::array<Vector, 9> coeffs;
		std::memcpy( coeffs.data(), data, sizeof( Vector ) * 9 );

		GraphicsInterface().UnLock( readBackBuffer );

		return coeffs;
	}

	agl::RefHandle<agl::Texture> GeneratePrefilteredSpecular( agl::RefHandle<agl::Texture> cubeMap )
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
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::TextureCube
		};

		auto prefiltered = agl::Texture::Create( trait, "PrefilteredSpecular" );
		EnqueueRenderTask(
			[prefiltered]()
			{
				prefiltered->Init();
			}
		);

		PrefilteredSpecularCS prefilteredSpecularCS;
		agl::RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( prefilteredSpecularCS );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( prefilteredSpecularCS );
		BindResource( shaderBindings, prefilteredSpecularCS.EnvMap(), cubeMap );

		auto linearSampler = StaticSamplerState<>::Get();
		BindResource( shaderBindings, prefilteredSpecularCS.EnvMapSampler(), linearSampler );

		auto commandList = GetCommandList();

		commandList.AddTransition( Transition( *prefiltered.Get(), agl::ResourceState::UnorderedAccess ) );

		for ( uint32 mipSlice = 0; mipSlice < trait.m_mipLevels; ++mipSlice )
		{
			BindResource( shaderBindings, prefilteredSpecularCS.Prefiltered(), prefiltered, mipSlice );
			SetShaderValue( commandList, prefilteredSpecularCS.Roughness(), static_cast<float>( mipSlice ) / trait.m_mipLevels );

			commandList.BindPipelineState( pso );
			commandList.BindShaderResources( shaderBindings );

			uint32 numGroupX = CalcAlignment<uint32>( width, 8 ) / 8;
			uint32 numGroupY = CalcAlignment<uint32>( height, 8 ) / 8;
			commandList.Dispatch( numGroupX, numGroupY, 6 );

			if ( width > 1 )
			{
				width >>= 1;
			}

			if ( height > 1 )
			{
				height >>= 1;
			}
		}

		commandList.AddTransition( Transition( *prefiltered.Get(), agl::ResourceState::PixelShaderResource ) );
		commandList.Commit();
		GetInterface<agl::IAgl>()->WaitGPU();

		return prefiltered;
	}
}
