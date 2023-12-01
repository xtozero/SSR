#include "IrradianceMapRendering.h"

#include "AbstractGraphicsInterface.h"
#include "CommandList.h"
#include "ComputePipelineState.h"
#include "GlobalShaders.h"
#include "PassProcessor.h"
#include "RenderOption.h"
#include "RenderTargetPool.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "SceneRenderer.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterUtils.h"
#include "TransitionUtils.h"
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

	REGISTER_GLOBAL_SHADER( DrawIrradianceMapVS, "./Assets/Shaders/IndirectLighting/IrradianceMap/VS_DrawIrradianceMap.asset" );
	REGISTER_GLOBAL_SHADER( DrawIrradianceMapGS, "./Assets/Shaders/IndirectLighting/IrradianceMap/GS_DrawIrradianceMap.asset" );
	REGISTER_GLOBAL_SHADER( DrawIrradianceMapPS, "./Assets/Shaders/IndirectLighting/IrradianceMap/PS_DrawIrradianceMap.asset" );
	REGISTER_GLOBAL_SHADER( IrradianceMapShCS, "./Assets/Shaders/IndirectLighting/IrradianceMap/CS_IrradianceMapSH.asset" );

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
		EnqueueRenderTask( [irradianceMap]()
			{
				irradianceMap->Init();
			} );

		auto commandList = GetCommandList();

		auto toRenderTarget = Transition( *irradianceMap.Get(), agl::ResourceState::RenderTarget);
		commandList.Transition( 1, &toRenderTarget );

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
				auto linearSampler = GraphicsInterface().FindOrCreate( SamplerOption() );
				RenderingShaderResource shaderResources;
				shaderResources.AddResource( "CubeMap", cubeMap->SRV() );
				shaderResources.AddResource( "LinearSampler", linearSampler.Resource() );

				DrawSnapshot& snapshot = *result;
				GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
				shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

				AddSingleDrawPass( snapshot );
			}
		}

		auto toShaderResource = Transition( *irradianceMap.Get(), agl::ResourceState::PixelShaderResource );
		commandList.Transition( 1, &toShaderResource );

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

		auto linearSampler = GraphicsInterface().FindOrCreate( SamplerOption() );

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
}
