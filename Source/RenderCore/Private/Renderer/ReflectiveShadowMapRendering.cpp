// http://www.klayge.org/material/3_12/GI/rsm.pdf

#include "ReflectiveShadowMapRendering.h"

#include "CommonRenderResource.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "PassProcessor.h"
#include "RenderTargetPool.h"
#include "RenderView.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "SceneRenderer.h"
#include "ShaderParameterUtils.h"
#include "TransitionUtils.h"
#include "VertexCollection.h"
#include "Viewport.h"

#include <cmath>
#include <numbers>
#include <random>

namespace rendercore
{
	class RSMsEvaluationPS : public GlobalShaderCommon<PixelShader, RSMsEvaluationPS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	REGISTER_GLOBAL_SHADER( RSMsEvaluationPS, "./Assets/Shaders/IndirectLighting/RSMs/PS_RSMsEvaluation.asset" );

	class RSMsDrawPassProcessor : public IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
	};

	std::optional<DrawSnapshot> RSMsDrawPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		PassShader passShader = {
			.m_vertexShader = FullScreenQuadVS(),
			.m_geometryShader = nullptr,
			.m_pixelShader = RSMsEvaluationPS()
		};

		DepthStencilOption RSMsDrawPassDepthOption;
		RSMsDrawPassDepthOption.m_depth.m_enable = false;
		RSMsDrawPassDepthOption.m_depth.m_writeDepth = false;

		PassRenderOption passRenderOption = {
			.m_depthStencilOption = &RSMsDrawPassDepthOption
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
	}

	void RSMsRenderer::PreRender( const RenderViewGroup& renderViewGroup )
	{
		std::pair<uint32, uint32> curRtSize = renderViewGroup.GetViewport().Size();
		if ( m_indirectIllumination == nullptr )
		{
			AllocTextureForIndirectIllumination( curRtSize );
		}
		else
		{
			auto texTrait = m_indirectIllumination->GetTrait();
			std::pair<uint32, uint32> oldRtSize = { texTrait.m_width, texTrait.m_height };

			if ( curRtSize != oldRtSize )
			{
				AllocTextureForIndirectIllumination( curRtSize );
			}
		}

		if ( m_samplingPattern == nullptr )
		{
			CreateSamplingPattern();
		}
	}

	void RSMsRenderer::Render( const RSMsRenderingParam& param, RenderingShaderResource& outRenderingShaderResource )
	{
		RSMsDrawPassProcessor rsmsDrawPassProcessor;

		PrimitiveSubMesh meshInfo;
		meshInfo.m_count = 3;

		auto result = rsmsDrawPassProcessor.Process( meshInfo );
		if ( result.has_value() == false )
		{
			return;
		}

		DrawSnapshot& snapshot = *result;
		
		// Update invalidated resources
		GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
		outRenderingShaderResource.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		RenderingShaderResource shaderResources;
		shaderResources.AddResource( "ViewSpaceDistance", param.m_viewSpaceDistance->SRV() );
		shaderResources.AddResource( "WorldNormal", param.m_worldNormal->SRV() );
		shaderResources.AddResource( "SamplingPattern", m_samplingPattern->SRV() );
		shaderResources.AddResource( "RSMsConstantParameters", m_constantParams.Resource() );
		shaderResources.AddResource( "BlackBorderSampler", m_blackBorderSampler.Resource() );

		agl::ResourceTransition beforeTransition[] = {
			Transition( *m_indirectIllumination.Get(), agl::ResourceState::RenderTarget ),
		};

		auto commandList = GetCommandList();
		commandList.Transition( std::extent_v<decltype( beforeTransition )>, beforeTransition );

		agl::RenderTargetView* rtv = m_indirectIllumination->RTV();
		commandList.BindRenderTargets( &rtv, 1, nullptr );

		for ( int32 i = 0; i < param.m_numShadowInfos; ++i )
		{
			ShadowInfo& shadowInfo = param.m_shadowInfos[i];
			const ShadowMapRenderTarget::ShadowMapList& shadowMaps = shadowInfo.ShadowMap().m_shadowMaps;
			if ( shadowMaps.size() < 4 )
			{
				continue;
			}

			shaderResources.AddResource( "ShadowDepthPassParameters", shadowInfo.ConstantBuffer().Resource() );
			shaderResources.AddResource( "RSMsNormal", shadowMaps[2]->SRV() );
			shaderResources.AddResource( "RSMsWorldPosition", shadowMaps[1]->SRV() );
			shaderResources.AddResource( "RSMsFlux", shadowMaps[3]->SRV() );
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

		agl::ResourceTransition afterTransition[] = {
			Transition( *m_indirectIllumination.Get(), agl::ResourceState::GenericRead ),
		};

		commandList.Transition( std::extent_v<decltype( afterTransition )>, afterTransition );

		outRenderingShaderResource.AddResource( "IndirectIllumination", m_indirectIllumination->SRV() );
	}

	void RSMsRenderer::AllocTextureForIndirectIllumination( const std::pair<uint32, uint32>& renderTargetSize )
	{
		agl::TextureTrait trait = {
			.m_width = renderTargetSize.first,
			.m_height = renderTargetSize.second,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R8G8B8A8_UNORM_SRGB,
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None,
			.m_clearValue = agl::ResourceClearValue{
				.m_color = { 0.f, 0.f, 0.f, 1.f }
			}
		};

		m_indirectIllumination = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "RSMs.Illumination" );

		SamplerOption blackBorderSampler;
		blackBorderSampler.m_addressU = agl::TextureAddressMode::Border;
		blackBorderSampler.m_addressV = agl::TextureAddressMode::Border;
		blackBorderSampler.m_addressW = agl::TextureAddressMode::Border;
		blackBorderSampler.m_borderColor = Color::Black;

		m_blackBorderSampler = GraphicsInterface().FindOrCreate( blackBorderSampler );
	}

	void RSMsRenderer::CreateSamplingPattern()
	{
		constexpr uint32 MaxNumSamplingPattern = 400;
		uint32 numSamplingPattern = std::min( MaxNumSamplingPattern, DefaultRenderCore::RSMsNumSampling() );

		agl::BufferTrait trait = {
			.m_stride = sizeof( Vector ),
			.m_count = numSamplingPattern,
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None,
			.m_format = agl::ResourceFormat::R32G32B32_FLOAT
		};

		Vector samplingPattern[MaxNumSamplingPattern] = {};

		std::random_device rd;
		std::mt19937 mt( rd() );

		std::uniform_real_distribution<float> dis( 0, 1 );

		for ( uint32 i = 0; i < numSamplingPattern; ++i )
		{
			float xi1 = dis( mt );
			float xi2 = dis( mt );

			// Equation 3.
			samplingPattern[i][0] = xi1 * std::sin( 2.f * std::numbers::pi_v<float> * xi2 );
			samplingPattern[i][1] = xi1 * std::cos( 2.f * std::numbers::pi_v<float> * xi2 );
			samplingPattern[i][2] = xi1;
		}

		m_samplingPattern = agl::Buffer::Create( trait, "RSMs.SamplingPattern", samplingPattern);
		assert( m_samplingPattern != nullptr );

		m_samplingPattern->Init();

		RSMsConstantParameters params = {
			.m_numSamplingPattern = numSamplingPattern,
			.m_maxRadius = DefaultRenderCore::RSMsMaxSamplingRadius(),
		};
		m_constantParams.Update( params );
	}
}
