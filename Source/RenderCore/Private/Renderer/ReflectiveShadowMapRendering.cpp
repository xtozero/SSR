// http://www.klayge.org/material/3_12/GI/rsm.pdf

#include "ReflectiveShadowMapRendering.h"

#include "CommonRenderResource.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GraphicsResourcePool.h"
#include "PassProcessor.h"
#include "RenderGraph.h"
#include "RenderView.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "SceneRenderer.h"
#include "StaticState.h"
#include "VertexCollection.h"
#include "Viewport.h"

#include <cmath>
#include <numbers>
#include <random>

namespace rendercore
{
	class RSMsEvaluationPS final : public GlobalShaderCommon<PixelShader, RSMsEvaluationPS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	REGISTER_GLOBAL_SHADER( RSMsEvaluationPS, "IndirectLighting/RSMs/PS_RSMsEvaluation.fx", agl::ShaderType::Pixel, "main" );

	class RSMsDrawPassProcessor final : public IPassProcessor
	{
	protected:
		virtual std::optional<DrawSnapshot> ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader ) override;
		virtual PassShader CollectPassShader( MaterialResource& material ) const override;
	};

	std::optional<DrawSnapshot> RSMsDrawPassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
		DepthStencilOption RSMsDrawPassDepthOption;
		RSMsDrawPassDepthOption.m_depth.m_enable = false;
		RSMsDrawPassDepthOption.m_depth.m_writeDepth = false;

		PassRenderOption passRenderOption = {
			.m_depthStencilOption = &RSMsDrawPassDepthOption
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
	}

	PassShader RSMsDrawPassProcessor::CollectPassShader( [[maybe_unused]] MaterialResource& material ) const
	{
		PassShader passShader = {
			.m_vertexShader = FullScreenQuadVS(),
			.m_pixelShader = RSMsEvaluationPS()
		};

		return passShader;
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

	RefHandle<agl::Texture> RSMsRenderer::Render( RenderGraph& renderGraph, const RSMsRenderingParam& param, const ResourceBinder& resourceBinder )
	{
		auto renderTarget = m_indirectIllumination.Get();
		auto rgRenderTarget = renderGraph.RegisterExternalResource( renderTarget );

		auto [width, height] = renderTarget->Size();

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgRenderTarget );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		auto rgViewSpaceDistance = renderGraph.RegisterExternalResource( param.m_viewSpaceDistance.Get() );
		auto rgWorldNormal = renderGraph.RegisterExternalResource( param.m_worldNormal.Get() );
		auto rgSamplingPattern = renderGraph.RegisterExternalResource( m_samplingPattern.Get() );

		for ( int32 i = 0; i < param.m_numShadowInfos; ++i )
		{
			const ShadowMapRenderTarget::ShadowMapList& shadowMaps = param.m_shadowInfos[i].ShadowMap().m_shadowMaps;
			size_t numShadowMaps = shadowMaps.size();
			if ( numShadowMaps < 4 )
			{
				continue;
			}

			BEGIN_RG_RESOURCE_STRUCT( RSMsRenderPassResource )
				DECLARE_RG_TEXTURE_PIXEL_SRV( viewSpaceDistance )
				DECLARE_RG_TEXTURE_PIXEL_SRV( worldNormal )
				DECLARE_RG_BUFFER_PIXEL_SRV( samplingPattern )
				DECLARE_RG_TEXTURE_PIXEL_SRV( rsmsWorldPosition )
				DECLARE_RG_TEXTURE_PIXEL_SRV( rsmsNormal )
				DECLARE_RG_TEXTURE_PIXEL_SRV( rsmsFlux )
			END_RG_RESOURCE_STRUCT();

			auto rgRsmsWorldPosition = renderGraph.RegisterExternalResource( shadowMaps[1].Get() );
			auto rgRsmsNormal = renderGraph.RegisterExternalResource( shadowMaps[2].Get() );
			auto rgRsmsFlux = renderGraph.RegisterExternalResource( shadowMaps[3].Get() );

			RSMsRenderPassResource passResource = {
				.m_viewSpaceDistance = rgViewSpaceDistance,
				.m_worldNormal = rgWorldNormal,
				.m_samplingPattern = rgSamplingPattern,
				.m_rsmsWorldPosition = rgRsmsWorldPosition,
				.m_rsmsNormal = rgRsmsNormal,
				.m_rsmsFlux = rgRsmsFlux,
			};

			renderGraph.AddPass(
				passResource,
				rasterOutput,
				[this, passResource, param, &resourceBinder, i]( CommandList& commandList )
				{
					RSMsDrawPassProcessor rsmsDrawPassProcessor;
					auto result = rsmsDrawPassProcessor.Process( FullScreenQuadDrawInfo() );
					if ( result.has_value() == false )
					{
						return;
					}

					DrawSnapshot& snapshot = *result;

					// Update invalidated resources
					GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
					resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

					ResourceBinder passResourceBinder;
					passResourceBinder.Add( StaticName( "ViewSpaceDistance" ), passResource.m_viewSpaceDistance->SRV() );
					passResourceBinder.Add( StaticName( "WorldNormal" ), passResource.m_worldNormal->SRV() );
					passResourceBinder.Add( StaticName( "SamplingPattern" ), passResource.m_samplingPattern->SRV() );
					passResourceBinder.Add( StaticName( "RSMsConstantParameters" ), m_shaderArguments->Resource() );
					passResourceBinder.Add( StaticName( "BlackBorderSampler" ), m_blackBorderSampler.Resource() );

					ShadowInfo& shadowInfo = param.m_shadowInfos[i];
					const ShadowMapRenderTarget::ShadowMapList& shadowMaps = shadowInfo.ShadowMap().m_shadowMaps;

					passResourceBinder.Add( StaticName( "ShadowDepthPassParameters" ), shadowInfo.GetShadowShaderArguments().Resource() );
					passResourceBinder.Add( StaticName( "RSMsWorldPosition" ), shadowMaps[1]->SRV() );
					passResourceBinder.Add( StaticName( "RSMsNormal" ), shadowMaps[2]->SRV() );
					passResourceBinder.Add( StaticName( "RSMsFlux" ), shadowMaps[3]->SRV() );
					passResourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

					AddSingleDrawPass( commandList, snapshot );
				} );
		}

		return m_indirectIllumination;
	}

	RSMsRenderer::RSMsRenderer()
	{
		m_shaderArguments = RSMsParameters::CreateShaderArguments();
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
			.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None,
			.m_clearValue = agl::ResourceClearValue{
				.m_color = { 0.f, 0.f, 0.f, 1.f }
			}
		};

		m_indirectIllumination = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "RSMs.Illumination" );

		m_blackBorderSampler = StaticSamplerState<agl::TextureFilter::MinMagMipLinear
			, agl::TextureAddressMode::Border
			, agl::TextureAddressMode::Border
			, agl::TextureAddressMode::Border
			, 0.f
			, agl::ComparisonFunc::Never
			, Color( 0, 0, 0, 255 )>::Get();
	}

	void RSMsRenderer::CreateSamplingPattern()
	{
		constexpr uint32 MaxNumSamplingPattern = 400;
		uint32 numSamplingPattern = std::min( MaxNumSamplingPattern, DefaultRenderCore::RSMsNumSampling() );

		agl::BufferTrait trait = {
			.m_stride = sizeof( Vector ),
			.m_count = numSamplingPattern,
			.m_access = agl::ResourceAccess::Default,
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

		m_samplingPattern = agl::Buffer::Create( trait, "RSMs.SamplingPattern", samplingPattern );
		assert( m_samplingPattern != nullptr );

		m_samplingPattern->Init();

		RSMsParameters params = {
			.NumSamplingPattern = numSamplingPattern,
			.MaxRadius = DefaultRenderCore::RSMsMaxSamplingRadius(),
		};

		m_shaderArguments->Update( params );
	}
}
