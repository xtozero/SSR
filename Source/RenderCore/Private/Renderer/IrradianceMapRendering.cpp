#include "IrradianceMapRendering.h"

#include "AbstractGraphicsInterface.h"
#include "CommandList.h"
#include "GlobalShaders.h"
#include "PassProcessor.h"
#include "RenderOption.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "SceneRenderer.h"
#include "TransitionUtils.h"
#include "VertexCollection.h"

#include <cassert>

namespace rendercore
{
	class DrawIrradianceMapVS : public GlobalShaderCommon<VertexShader, DrawIrradianceMapVS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	public:
		DrawIrradianceMapVS() = default;
	};

	class DrawIrradianceMapGS : public GlobalShaderCommon<GeometryShader, DrawIrradianceMapGS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	public:
		DrawIrradianceMapGS() = default;
	};

	class DrawIrradianceMapPS : public GlobalShaderCommon<PixelShader, DrawIrradianceMapPS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	public:
		DrawIrradianceMapPS() = default;
	};

	REGISTER_GLOBAL_SHADER( DrawIrradianceMapVS, "./Assets/Shaders/IndirectLighting/IrradianceMap/VS_DrawIrradianceMap.asset" );
	REGISTER_GLOBAL_SHADER( DrawIrradianceMapGS, "./Assets/Shaders/IndirectLighting/IrradianceMap/GS_DrawIrradianceMap.asset" );
	REGISTER_GLOBAL_SHADER( DrawIrradianceMapPS, "./Assets/Shaders/IndirectLighting/IrradianceMap/PS_DrawIrradianceMap.asset" );

	class IrradianceMapGenerateProcessor : public IPassProcessor
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

		auto irradianceMap = agl::Texture::Create( trait );
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

				VisibleDrawSnapshot visibleSnapshot = {
					.m_primitiveId = 0,
					.m_primitiveIdOffset = 0,
					.m_numInstance = 1,
					.m_snapshotBucketId = -1,
					.m_drawSnapshot = &snapshot,
				};

				VertexBuffer emptyPrimitiveID;
				CommitDrawSnapshot( commandList, visibleSnapshot, emptyPrimitiveID );
			}
		}

		auto toShaderResource = Transition( *irradianceMap.Get(), agl::ResourceState::PixelShaderResource );
		commandList.Transition( 1, &toShaderResource );

		commandList.Commit();
		GetInterface<agl::IAgl>()->WaitGPU(); // Test

		return irradianceMap;
	}
}
