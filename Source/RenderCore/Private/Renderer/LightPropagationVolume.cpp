#include "LightPropagationVolume.h"

#include "AbstractGraphicsInterface.h"
#include "CommonRenderResource.h"
#include "ComputePipelineState.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "GraphicsResourcePool.h"
#include "LightProxy.h"
#include "PassProcessor.h"
#include "RenderGraph.h"
#include "RenderView.h"
#include "Scene/IScene.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "SceneRenderer.h"
#include "ShaderParameterUtils.h"
#include "StaticState.h"
#include "VertexCollection.h"
#include "Viewport.h"

namespace rendercore
{
	class ClearLpvCS final : public GlobalShaderCommon<ComputeShader, ClearLpvCS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	private:
		DEFINE_SHADER_PARAM( CoeffR );
		DEFINE_SHADER_PARAM( CoeffG );
		DEFINE_SHADER_PARAM( CoeffB );
		DEFINE_SHADER_PARAM( CoeffOcclusion );
	};

	class DownSampleRSMsCS final : public GlobalShaderCommon<ComputeShader, DownSampleRSMsCS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	private:
		DEFINE_SHADER_PARAM( LPVCommonParameters ); // b5

		DEFINE_SHADER_PARAM( RSMsDimensions );
		DEFINE_SHADER_PARAM( KernelSize );
		DEFINE_SHADER_PARAM( ToLightDir );

		DEFINE_SHADER_PARAM( RSMsNormal );
		DEFINE_SHADER_PARAM( RSMsWorldPosition );
		DEFINE_SHADER_PARAM( RSMsFlux );

		DEFINE_SHADER_PARAM( OutRSMsNormal );
		DEFINE_SHADER_PARAM( OutRSMsWorldPosition );
		DEFINE_SHADER_PARAM( OutRSMsFlux );
	};

	class LightInjectionVS final : public GlobalShaderCommon<VertexShader, LightInjectionVS>
	{
	private:
		DEFINE_SHADER_PARAM( RSMsDimensions );
		DEFINE_SHADER_PARAM( SurfelArea );
	};

	class LightInjectionGS final : public GlobalShaderCommon<GeometryShader, LightInjectionGS>
	{};

	class LightInjectionPS final : public GlobalShaderCommon<PixelShader, LightInjectionPS>
	{};

	class GeometryInjectionVS final : public GlobalShaderCommon<VertexShader, GeometryInjectionVS>
	{
	private:
		DEFINE_SHADER_PARAM( RSMsDimensions );
		DEFINE_SHADER_PARAM( LightDirection );
	};

	class GeometryInjectionGS final : public GlobalShaderCommon<GeometryShader, GeometryInjectionGS>
	{};

	class GeometryInjectionPS final : public GlobalShaderCommon<PixelShader, GeometryInjectionPS>
	{};

	class LightPropagationCS final : public GlobalShaderCommon<ComputeShader, LightPropagationCS>
	{
	private:
		DEFINE_SHADER_PARAM( LPVCommonParameters ); // b5

		DEFINE_SHADER_PARAM( CoeffR );
		DEFINE_SHADER_PARAM( CoeffG );
		DEFINE_SHADER_PARAM( CoeffB );
		DEFINE_SHADER_PARAM( OutCoeffR );
		DEFINE_SHADER_PARAM( OutCoeffG );
		DEFINE_SHADER_PARAM( OutCoeffB );

		DEFINE_SHADER_PARAM( CoeffOcclusion );
		DEFINE_SHADER_PARAM( BlackBorderLinearSampler );

		DEFINE_SHADER_PARAM( InterationCount );
	};

	class RenderLpvPS final : public GlobalShaderCommon<PixelShader, RenderLpvPS>
	{};

	REGISTER_GLOBAL_SHADER( ClearLpvCS, "./Assets/Shaders/IndirectLighting/LPV/CS_ClearLPV.asset" );
	REGISTER_GLOBAL_SHADER( DownSampleRSMsCS, "./Assets/Shaders/IndirectLighting/LPV/CS_DownSampleRSMs.asset" );
	REGISTER_GLOBAL_SHADER( LightInjectionVS, "./Assets/Shaders/IndirectLighting/LPV/VS_LightInjection.asset" );
	REGISTER_GLOBAL_SHADER( LightInjectionGS, "./Assets/Shaders/IndirectLighting/LPV/GS_LightInjection.asset" );
	REGISTER_GLOBAL_SHADER( LightInjectionPS, "./Assets/Shaders/IndirectLighting/LPV/PS_LightInjection.asset" );
	REGISTER_GLOBAL_SHADER( GeometryInjectionVS, "./Assets/Shaders/IndirectLighting/LPV/VS_GeometryInjection.asset" );
	REGISTER_GLOBAL_SHADER( GeometryInjectionGS, "./Assets/Shaders/IndirectLighting/LPV/GS_GeometryInjection.asset" );
	REGISTER_GLOBAL_SHADER( GeometryInjectionPS, "./Assets/Shaders/IndirectLighting/LPV/PS_GeometryInjection.asset" );
	REGISTER_GLOBAL_SHADER( LightPropagationCS, "./Assets/Shaders/IndirectLighting/LPV/CS_LightPropagation.asset" );
	REGISTER_GLOBAL_SHADER( RenderLpvPS, "./Assets/Shaders/IndirectLighting/LPV/PS_RenderLPV.asset" );

	class LightInjectionPassProcessor final : public IPassProcessor
	{
	protected:
		virtual std::optional<DrawSnapshot> ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader ) override
		{
			DepthStencilOption passDepthOption;
			passDepthOption.m_depth.m_enable = false;
			passDepthOption.m_depth.m_writeDepth = false;

			BlendOption passBlendOption;
			for ( int32 i = 0; i < 3; ++i )
			{
				passBlendOption.m_renderTarget[i].m_blendEnable = true;
				passBlendOption.m_renderTarget[i].m_srcBlend = agl::Blend::One;
				passBlendOption.m_renderTarget[i].m_destBlend = agl::Blend::One;
				passBlendOption.m_renderTarget[i].m_blendOp = agl::BlendOp::Add;
				passBlendOption.m_renderTarget[i].m_srcBlendAlpha = agl::Blend::One;
				passBlendOption.m_renderTarget[i].m_destBlendAlpha = agl::Blend::One;
				passBlendOption.m_renderTarget[i].m_blendOpAlpha = agl::BlendOp::Add;
			}

			PassRenderOption passRenderOption = {
				.m_primitive = agl::ResourcePrimitive::Pointlist,
				.m_blendOption = &passBlendOption,
				.m_depthStencilOption = &passDepthOption
			};

			return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
		}
		
		virtual PassShader CollectPassShader( [[maybe_unused]] MaterialResource& material ) const override
		{
			PassShader passShader = {
				.m_vertexShader = LightInjectionVS(),
				.m_geometryShader = LightInjectionGS(),
				.m_pixelShader = LightInjectionPS()
			};

			return passShader;
		}
	};

	class GeometryInjectionPassProcessor final : public IPassProcessor
	{
	protected:
		virtual std::optional<DrawSnapshot> ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader ) override
		{
			DepthStencilOption passDepthOption;
			passDepthOption.m_depth.m_enable = false;
			passDepthOption.m_depth.m_writeDepth = false;

			BlendOption passBlendOption;
			for ( int32 i = 0; i < 1; ++i )
			{
				passBlendOption.m_renderTarget[i].m_blendEnable = true;
				passBlendOption.m_renderTarget[i].m_srcBlend = agl::Blend::One;
				passBlendOption.m_renderTarget[i].m_destBlend = agl::Blend::One;
				passBlendOption.m_renderTarget[i].m_blendOp = agl::BlendOp::Add;
				passBlendOption.m_renderTarget[i].m_srcBlendAlpha = agl::Blend::One;
				passBlendOption.m_renderTarget[i].m_destBlendAlpha = agl::Blend::One;
				passBlendOption.m_renderTarget[i].m_blendOpAlpha = agl::BlendOp::Add;
			}

			PassRenderOption passRenderOption = {
				.m_primitive = agl::ResourcePrimitive::Pointlist,
				.m_blendOption = &passBlendOption,
				.m_depthStencilOption = &passDepthOption
			};

			return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
		}
		
		virtual PassShader CollectPassShader( [[maybe_unused]] MaterialResource& material ) const override
		{
			PassShader passShader = {
				.m_vertexShader = GeometryInjectionVS(),
				.m_geometryShader = GeometryInjectionGS(),
				.m_pixelShader = GeometryInjectionPS()
			};

			return passShader;
		}
	};

	class LpvRenderPassProcessor final : public IPassProcessor
	{
	protected:
		virtual std::optional<DrawSnapshot> ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader ) override
		{
			DepthStencilOption RSMsDrawPassDepthOption;
			RSMsDrawPassDepthOption.m_depth.m_enable = false;
			RSMsDrawPassDepthOption.m_depth.m_writeDepth = false;

			PassRenderOption passRenderOption = {
				.m_depthStencilOption = &RSMsDrawPassDepthOption
			};

			return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
		}

		virtual PassShader CollectPassShader( [[maybe_unused]] MaterialResource& material ) const override
		{
			PassShader passShader = {
				.m_vertexShader = FullScreenQuadVS(),
				.m_pixelShader = RenderLpvPS()
			};

			return passShader;
		}
	};

	void LightPropagationVolume::Prepare( RenderGraph& renderGraph, const RenderViewGroup& renderViewGroup )
	{
		std::pair<uint32, uint32> curRtSize = renderViewGroup.GetViewport().Size();

		InitResource( curRtSize );
		ClearLPV( renderGraph );
	}

	void LightPropagationVolume::InjectLight( RenderGraph& renderGraph, IScene& scene, RenderThreadFrameData<ShadowInfo>& shadowInfos )
	{
		for ( ShadowInfo& shadowInfo : shadowInfos )
		{
			const ShadowMapRenderTarget& shadowMapRT = shadowInfo.ShadowMap();
			if ( shadowMapRT.m_shadowMaps.size() < 4 )
			{
				continue;
			}

			const LightSceneInfo* lightSceneInfo = shadowInfo.GetLightSceneInfo();
			if ( lightSceneInfo == nullptr )
			{
				continue;
			}

			LpvLightInjectionParameters params = {
				.lightInfo = lightSceneInfo,
				.m_viewShaderArguments = scene.GetViewShaderArguments().Resource(),
				.m_shadowDepthPassParameters = shadowInfo.GetShadowShaderArguments().Resource(),
				.m_rsmTextures = {
					.m_worldPosition = shadowMapRT.m_shadowMaps[1],
					.m_normal = shadowMapRT.m_shadowMaps[2],
					.m_flux = shadowMapRT.m_shadowMaps[3],
				},
				.m_surfelAreas = { 0.06f * 0.07f, 0.06f } // For now, the hardcoded
			};

			bool isValid = params.lightInfo != nullptr
				&& params.m_viewShaderArguments.Get() != nullptr
				&& params.m_shadowDepthPassParameters.Get() != nullptr
				&& params.m_rsmTextures.m_worldPosition != nullptr
				&& params.m_rsmTextures.m_normal != nullptr
				&& params.m_rsmTextures.m_flux != nullptr;

			if ( isValid == false )
			{
				continue;
			}

			LpvRSMTextures downSampledTex = DownSampleRSMs( renderGraph, *params.lightInfo, params.m_rsmTextures );
			InjectToLPV( renderGraph, params, downSampledTex );
		}
	}

	void LightPropagationVolume::Propagate( RenderGraph& renderGraph )
	{
		LPVTextures tempTextures = AllocVolumeTextures( false );

		auto rgCoeffR = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffR.Get() );
		auto rgCoeffG = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffG.Get() );
		auto rgCoeffB = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffB.Get() );
		auto rgCoeffOcclusion = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffOcclusion.Get() );
		auto rgTempCoeffR = renderGraph.RegisterExternalResource( tempTextures.m_coeffR.Get() );
		auto rgTempCoeffG = renderGraph.RegisterExternalResource( tempTextures.m_coeffG.Get() );
		auto rgTempCoeffB = renderGraph.RegisterExternalResource( tempTextures.m_coeffB.Get() );

		BEGIN_RG_RESOURCE_STRUCT( PropagatePassResource )
			DECLARE_RG_TEXTURE_UAV( coeffR )
			DECLARE_RG_TEXTURE_UAV( coeffG )
			DECLARE_RG_TEXTURE_UAV( coeffB )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( coeffOcclusion )
			DECLARE_RG_TEXTURE_UAV( tempCoeffR )
			DECLARE_RG_TEXTURE_UAV( tempCoeffG )
			DECLARE_RG_TEXTURE_UAV( tempCoeffB )
		END_RG_RESOURCE_STRUCT();

		PropagatePassResource passResource = {
			.m_coeffR = rgCoeffR,
			.m_coeffG = rgCoeffG,
			.m_coeffB = rgCoeffB,
			.m_coeffOcclusion = rgCoeffOcclusion,
			.m_tempCoeffR = rgTempCoeffR,
			.m_tempCoeffG = rgTempCoeffG,
			.m_tempCoeffB = rgTempCoeffB
		};

		renderGraph.AddPass( 
			passResource,
			[this, passResource]( ComputeCommandList& commandList ) mutable
			{
				LightPropagationCS lightPropagationCS;

				agl::ShaderBindings shaderBindings = CreateShaderBindings( lightPropagationCS );
				BindResource( shaderBindings, lightPropagationCS.LPVCommonParameters(), m_lpvCommon.Get() );
				BindResource( shaderBindings, lightPropagationCS.CoeffOcclusion(), passResource.m_coeffOcclusion->Get() );

				auto blackBorderLinearSampler = StaticSamplerState<agl::TextureFilter::MinMagMipLinear
					, agl::TextureAddressMode::Border
					, agl::TextureAddressMode::Border
					, agl::TextureAddressMode::Border
					, 0.f
					, agl::ComparisonFunc::Never
					, Color( 0, 0, 0, 0 )>::Get();
				BindResource( shaderBindings, lightPropagationCS.BlackBorderLinearSampler(), blackBorderLinearSampler );

				RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( lightPropagationCS );

				// [numthreads(4, 4, 32)] -> Dispatch( 32 / 8, 32 / 8, 32 / 1 )
				for ( uint32 i = 0; i < DefaultRenderCore::NumLpvIteration(); ++i )
				{
					BindResource( shaderBindings, lightPropagationCS.CoeffR(), passResource.m_coeffR->Get() );
					BindResource( shaderBindings, lightPropagationCS.CoeffG(), passResource.m_coeffG->Get() );
					BindResource( shaderBindings, lightPropagationCS.CoeffB(), passResource.m_coeffB->Get() );
					BindResource( shaderBindings, lightPropagationCS.OutCoeffR(), passResource.m_tempCoeffR->Get() );
					BindResource( shaderBindings, lightPropagationCS.OutCoeffG(), passResource.m_tempCoeffG->Get() );
					BindResource( shaderBindings, lightPropagationCS.OutCoeffB(), passResource.m_tempCoeffB->Get() );

					SetShaderValue( commandList, lightPropagationCS.InterationCount(), i + 1 );

					commandList.BindPipelineState( pso.Get() );
					commandList.BindShaderResources( shaderBindings );
					commandList.Dispatch( 4, 4, 32 );

					std::swap( passResource.m_coeffR, passResource.m_tempCoeffR );
					std::swap( passResource.m_coeffG, passResource.m_tempCoeffG );
					std::swap( passResource.m_coeffB, passResource.m_tempCoeffB );
				}
			} );

		if ( ( DefaultRenderCore::NumLpvIteration() % 2 ) == 1 )
		{
			m_lpvTextures = tempTextures;
		}
	}

	RefHandle<agl::Texture> LightPropagationVolume::Render( RenderGraph& renderGraph, const LpvRenderingParameters& param, const ResourceBinder& resourceBinder )
	{
		auto rgViewSpaceDistance = renderGraph.RegisterExternalResource( param.m_viewSpaceDistance.Get() );
		auto rgWorldNormal = renderGraph.RegisterExternalResource( param.m_worldNormal.Get() );
		auto rgCoeffR = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffR.Get() );
		auto rgCoeffG = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffG.Get() );
		auto rgCoeffB = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffB.Get() );

		BEGIN_RG_RESOURCE_STRUCT( LPVRenderPassResource )
			DECLARE_RG_TEXTURE_PIXEL_SRV( viewSpaceDistance )
			DECLARE_RG_TEXTURE_PIXEL_SRV( worldNormal )
			DECLARE_RG_TEXTURE_PIXEL_SRV( coeffR )
			DECLARE_RG_TEXTURE_PIXEL_SRV( coeffG )
			DECLARE_RG_TEXTURE_PIXEL_SRV( coeffB )
		END_RG_RESOURCE_STRUCT();

		LPVRenderPassResource passResource = {
			.m_viewSpaceDistance = rgViewSpaceDistance,
			.m_worldNormal = rgWorldNormal,
			.m_coeffR = rgCoeffR,
			.m_coeffG = rgCoeffG,
			.m_coeffB = rgCoeffB
		};

		auto renderTarget = m_indirectIllumination.Get();
		auto rgRenderTarget = renderGraph.RegisterExternalResource( renderTarget );

		auto [width, height] = renderTarget->Size();

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgRenderTarget );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		renderGraph.AddPass(
			passResource,
			rasterOutput,
			[this, passResource, &resourceBinder]( CommandList& commandList )
			{
				LpvRenderPassProcessor lpvDrawPassProcessor;
				auto result = lpvDrawPassProcessor.Process( FullScreenQuadDrawInfo() );
				if ( result.has_value() == false )
				{
					return;
				}

				DrawSnapshot& snapshot = *result;

				// Update invalidated resources
				GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
				resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

				auto blackBorderSampler = StaticSamplerState<agl::TextureFilter::MinMagMipLinear
					, agl::TextureAddressMode::Border
					, agl::TextureAddressMode::Border
					, agl::TextureAddressMode::Border
					, 0.f
					, agl::ComparisonFunc::Never
					, Color( 0, 0, 0, 255 )>::Get();

				ResourceBinder passResourceBinder;
				passResourceBinder.Add( "ViewSpaceDistance", passResource.m_viewSpaceDistance->SRV() );
				passResourceBinder.Add( "WorldNormal", passResource.m_worldNormal->SRV() );
				passResourceBinder.Add( "CoeffR", passResource.m_coeffR->SRV() );
				passResourceBinder.Add( "CoeffG", passResource.m_coeffG->SRV() );
				passResourceBinder.Add( "CoeffB", passResource.m_coeffB->SRV() );
				passResourceBinder.Add( "BlackBorderSampler", blackBorderSampler.Resource() );
				passResourceBinder.Add( "LPVCommonParameters", m_lpvCommon.Get() );
				passResourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

				AddSingleDrawPass( commandList, snapshot );
			} );

		return m_indirectIllumination;
	}

	void LightPropagationVolume::AllocTextureForIndirectIllumination( const std::pair<uint32, uint32>& renderTargetSize )
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

		m_indirectIllumination = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "LPV.Illumination" );
	}

	LightPropagationVolume::LPVTextures LightPropagationVolume::AllocVolumeTextures( bool allocForOcclusion )
	{
		LPVTextures volumeTextures;

		agl::TextureTrait trait = {
				.m_width = 32,
				.m_height = 32,
				.m_depth = 32,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RenderTarget | agl::ResourceBindType::RandomAccess,
				.m_miscFlag = agl::ResourceMisc::Texture3D
		};

		volumeTextures.m_coeffR = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "LPV.Coeff.R" );
		volumeTextures.m_coeffG = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "LPV.Coeff.G" );
		volumeTextures.m_coeffB = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "LPV.Coeff.B" );

		if ( allocForOcclusion )
		{
			volumeTextures.m_coeffOcclusion = GraphicsResourcePool::GetInstance().FindFreeTexture( trait, "LPV.Coeff.Occlusion" );
		}

		return volumeTextures;
	}

	void LightPropagationVolume::InitResource( const std::pair<uint32, uint32>& renderTargetSize )
	{
		if ( m_lpvCommon.Get() == nullptr )
		{
			agl::BufferTrait trait = {
				.m_stride = sizeof( Vector4 ) + sizeof( Vector4 ),
				.m_count = 1,
				.m_access = agl::ResourceAccessFlag::Upload,
				.m_bindType = agl::ResourceBindType::ConstantBuffer,
				.m_format = agl::ResourceFormat::Unknown
			};

			m_lpvCommon = agl::Buffer::Create( trait, "lpvCommon" );
			m_lpvCommon->Init();

			auto dest = static_cast<uint8*>( GraphicsInterface().Lock( m_lpvCommon.Get() ).m_data );

			Vector4 textureDimension( 32.f, 32.f, 32.f, 32.f );
			memcpy( dest, &textureDimension, sizeof( Vector4 ) );
			dest += sizeof( Vector4 );

			Vector4 cellSize( 4.f, 4.f, 4.f, 4.f );
			memcpy( dest, &cellSize, sizeof( Vector4 ) );

			GraphicsInterface().UnLock( m_lpvCommon.Get() );
		}

		if ( m_lpvTextures.m_coeffR.Get() == nullptr
			|| m_lpvTextures.m_coeffG.Get() == nullptr
			|| m_lpvTextures.m_coeffB.Get() == nullptr
			|| m_lpvTextures.m_coeffOcclusion.Get() == nullptr )
		{
			m_lpvTextures = AllocVolumeTextures( true );
		}

		if( m_indirectIllumination == nullptr )
		{
			AllocTextureForIndirectIllumination( renderTargetSize );
		}
		else
		{
			auto texTrait = m_indirectIllumination->GetTrait();
			std::pair<uint32, uint32> oldRtSize = { texTrait.m_width, texTrait.m_height };

			if ( renderTargetSize != oldRtSize )
			{
				AllocTextureForIndirectIllumination( renderTargetSize );
			}
		}
	}

	void LightPropagationVolume::ClearLPV( RenderGraph& renderGraph )
	{
		auto rgCoeffR = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffR.Get() );
		auto rgCoeffG = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffG.Get() );
		auto rgCoeffB = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffB.Get() );
		auto rgCoeffOcclusion = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffOcclusion.Get() );

		BEGIN_RG_RESOURCE_STRUCT( ClearLPVPassResouce )
			DECLARE_RG_TEXTURE_UAV( coeffR )
			DECLARE_RG_TEXTURE_UAV( coeffG )
			DECLARE_RG_TEXTURE_UAV( coeffB )
			DECLARE_RG_TEXTURE_UAV( coeffOcclusion )
		END_RG_RESOURCE_STRUCT();

		ClearLPVPassResouce passResouce = {
			.m_coeffR = rgCoeffR,
			.m_coeffG = rgCoeffG,
			.m_coeffB = rgCoeffB,
			.m_coeffOcclusion = rgCoeffOcclusion
		};

		renderGraph.AddPass(
			passResouce,
			[passResouce]( ComputeCommandList& commandList )
			{
				ClearLpvCS clearLpvCS;

				agl::ShaderBindings shaderBindings = CreateShaderBindings( clearLpvCS );
				BindResource( shaderBindings, clearLpvCS.CoeffR(), passResouce.m_coeffR->Get() );
				BindResource( shaderBindings, clearLpvCS.CoeffG(), passResouce.m_coeffG->Get() );
				BindResource( shaderBindings, clearLpvCS.CoeffB(), passResouce.m_coeffB->Get() );
				BindResource( shaderBindings, clearLpvCS.CoeffOcclusion(), passResouce.m_coeffOcclusion->Get() );

				RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( clearLpvCS );

				commandList.BindPipelineState( pso.Get() );
				commandList.BindShaderResources( shaderBindings );

				// [numthreads(8, 8, 8)] -> Dispatch( 32 / 4, 32 / 4, 32 / 4 )
				commandList.Dispatch( 8, 8, 8 );
			} );
	}

	LpvRSMTextures LightPropagationVolume::DownSampleRSMs( RenderGraph& renderGraph, const LightSceneInfo& lightInfo, const LpvRSMTextures& rsmTextures )
	{
		LpvRSMTextures downSampledTex;

		// Create textures
		{
			agl::TextureTrait positionMapTrait = {
				.m_width = 512,
				.m_height = 512,
				.m_depth = CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
				.m_miscFlag = agl::ResourceMisc::None,
			};

			downSampledTex.m_worldPosition = agl::Texture::Create( positionMapTrait, "LPV.DownSampled.RSMs.Position" );
			downSampledTex.m_worldPosition->Init();

			agl::TextureTrait normalMapTrait = {
				.m_width = 512,
				.m_height = 512,
				.m_depth = CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R10G10B10A2_UNORM,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
				.m_miscFlag = agl::ResourceMisc::None,
			};

			downSampledTex.m_normal = agl::Texture::Create( normalMapTrait, "LPV.DownSampled.RSMs.Normal" );
			downSampledTex.m_normal->Init();

			agl::TextureTrait fluxMapTrait = {
				.m_width = 512,
				.m_height = 512,
				.m_depth = CascadeShadowSetting::MAX_CASCADE_NUM, // Cascade map count, Right now, it's fixed constant.
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
				.m_miscFlag = agl::ResourceMisc::None,
			};

			downSampledTex.m_flux = agl::Texture::Create( fluxMapTrait, "LPV.DownSampled.RSMs.Flux" );
			downSampledTex.m_flux->Init();
		}

		const Vector& lightDir = -lightInfo.Proxy()->GetLightProperty().m_direction;

		auto rgSrcWorldPosition = renderGraph.RegisterExternalResource( rsmTextures.m_worldPosition.Get() );
		auto rgSrcNormal = renderGraph.RegisterExternalResource( rsmTextures.m_normal.Get() );
		auto rgSrcFlux = renderGraph.RegisterExternalResource( rsmTextures.m_flux.Get() );
		auto rgDstWorldPosition = renderGraph.RegisterExternalResource( downSampledTex.m_worldPosition.Get() );
		auto rgDstNormal = renderGraph.RegisterExternalResource( downSampledTex.m_normal.Get() );
		auto rgDstFlux = renderGraph.RegisterExternalResource( downSampledTex.m_flux.Get() );

		BEGIN_RG_RESOURCE_STRUCT( DownsamplePassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( srcWorldPosition )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( srcNormal )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( srcFlux )
			DECLARE_RG_TEXTURE_UAV( dstWorldPosition )
			DECLARE_RG_TEXTURE_UAV( dstNormal )
			DECLARE_RG_TEXTURE_UAV( dstFlux )
		END_RG_RESOURCE_STRUCT();

		DownsamplePassResource passResource = {
			.m_srcWorldPosition = rgSrcWorldPosition,
			.m_srcNormal = rgSrcNormal,
			.m_srcFlux = rgSrcFlux,
			.m_dstWorldPosition = rgDstWorldPosition,
			.m_dstNormal = rgDstNormal,
			.m_dstFlux = rgDstFlux
		};

		renderGraph.AddPass(
			passResource,
			[this, passResource, lightDir]( ComputeCommandList& commandList )
			{
				DownSampleRSMsCS downSampleRSMsCS;
				RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( downSampleRSMsCS );

				commandList.BindPipelineState( pso.Get() );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( downSampleRSMsCS );
				BindResource( shaderBindings, downSampleRSMsCS.LPVCommonParameters(), m_lpvCommon.Get() );
				BindResource( shaderBindings, downSampleRSMsCS.RSMsWorldPosition(), passResource.m_srcWorldPosition->Get() );
				BindResource( shaderBindings, downSampleRSMsCS.RSMsNormal(), passResource.m_srcNormal->Get() );
				BindResource( shaderBindings, downSampleRSMsCS.RSMsFlux(), passResource.m_srcFlux->Get() );
				BindResource( shaderBindings, downSampleRSMsCS.OutRSMsWorldPosition(), passResource.m_dstWorldPosition->Get() );
				BindResource( shaderBindings, downSampleRSMsCS.OutRSMsNormal(), passResource.m_dstNormal->Get() );
				BindResource( shaderBindings, downSampleRSMsCS.OutRSMsFlux(), passResource.m_dstFlux->Get() );

				const agl::TextureTrait& rsmTextureTrait = passResource.m_srcWorldPosition->GetTrait();
				uint32 dimensions[] = { rsmTextureTrait.m_width, rsmTextureTrait.m_height, rsmTextureTrait.m_depth };

				SetShaderValue( commandList, downSampleRSMsCS.RSMsDimensions(), dimensions );

				uint32 kernelSize = rsmTextureTrait.m_width / 512;
				SetShaderValue( commandList, downSampleRSMsCS.KernelSize(), kernelSize );
				SetShaderValue( commandList, downSampleRSMsCS.ToLightDir(), lightDir );

				commandList.BindShaderResources( shaderBindings );

				// [numthreads(8, 8, 1)] -> Dispatch( 512 / 8, 512 / 8, CascadeShadowSetting::MAX_CASCADE_NUM / 1 )
				commandList.Dispatch( 64, 64, CascadeShadowSetting::MAX_CASCADE_NUM );
			} );

		return downSampledTex;
	}

	void LightPropagationVolume::InjectToLPV( RenderGraph& renderGraph, const LpvLightInjectionParameters& params, const LpvRSMTextures& downSampledTex )
	{
		auto rgWorldPosition = renderGraph.RegisterExternalResource( downSampledTex.m_worldPosition.Get() );
		auto rgNormal = renderGraph.RegisterExternalResource( downSampledTex.m_normal.Get() );
		auto rgFlux = renderGraph.RegisterExternalResource( downSampledTex.m_flux.Get() );

		BEGIN_RG_RESOURCE_STRUCT( InjectionPassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( worldPosition )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( normal )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( flux )
		END_RG_RESOURCE_STRUCT();

		InjectionPassResource passResouce = {
			.m_worldPosition = rgWorldPosition,
			.m_normal = rgNormal,
			.m_flux = rgFlux
		};

		{
			auto rgcoeffR = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffR.Get() );
			auto rgcoeffG = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffG.Get() );
			auto rgcoeffB = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffB.Get() );

			RasterOutput rasterOutput;
			rasterOutput.SetRenderTarget( 0, rgcoeffR );
			rasterOutput.SetRenderTarget( 1, rgcoeffG );
			rasterOutput.SetRenderTarget( 2, rgcoeffB );
			rasterOutput.SetViewport( 32, 32 );
			rasterOutput.SetScissorRect( 32, 32 );

			renderGraph.AddPass(
				passResouce,
				rasterOutput,
				[this, passResouce, params]( CommandList& commandList )
				{
					LightInjectionPassProcessor lightInjectionPassProcessor;

					PrimitiveSubMesh meshInfo;
					meshInfo.m_count = 512 * 512 * CascadeShadowSetting::MAX_CASCADE_NUM;

					auto lightInjectionPass = lightInjectionPassProcessor.Process( meshInfo );
					if ( lightInjectionPass.has_value() )
					{
						const agl::TextureTrait& rsmTextureTrait = passResouce.m_worldPosition->GetTrait();
						uint32 dimensions[] = { rsmTextureTrait.m_width, rsmTextureTrait.m_height, rsmTextureTrait.m_depth };
						SetShaderValue( commandList, LightInjectionVS().RSMsDimensions(), dimensions );

						Vector4 vSurfelArea[CascadeShadowSetting::MAX_CASCADE_NUM] = {};
						for ( int32 i = 0; i < CascadeShadowSetting::MAX_CASCADE_NUM; ++i )
						{
							float surfelArea = params.m_surfelAreas[i];
							vSurfelArea[i] = Vector4( surfelArea, surfelArea, surfelArea, surfelArea );
						}
						SetShaderValue( commandList, LightInjectionVS().SurfelArea(), vSurfelArea );

						ResourceBinder resourceBinder;
						resourceBinder.Add( "SceneViewParameters", params.m_viewShaderArguments.Get() );
						resourceBinder.Add( "LPVCommonParameters", m_lpvCommon.Get() );
						resourceBinder.Add( "ShadowDepthPassParameters", params.m_shadowDepthPassParameters.Get() );

						resourceBinder.Add( "RSMsWorldPosition", passResouce.m_worldPosition->SRV() );
						resourceBinder.Add( "RSMsNormal", passResouce.m_normal->SRV() );
						resourceBinder.Add( "RSMsFlux", passResouce.m_flux->SRV() );

						DrawSnapshot& snapshot = *lightInjectionPass;
						GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
						resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

						AddSingleDrawPass( commandList, snapshot );
					}
				} );
		}

		{
			auto rgCoeffOcclusion = renderGraph.RegisterExternalResource( m_lpvTextures.m_coeffOcclusion.Get() );

			RasterOutput rasterOutput;
			rasterOutput.SetRenderTarget( 0, rgCoeffOcclusion );
			rasterOutput.SetViewport( 32, 32 );
			rasterOutput.SetScissorRect( 32, 32 );

			renderGraph.AddPass(
				passResouce,
				rasterOutput,
				[this, passResouce, params]( CommandList& commandList )
				{
					GeometryInjectionPassProcessor geometryInjectionPassProcessor;

					PrimitiveSubMesh meshInfo;
					meshInfo.m_count = 512 * 512 * CascadeShadowSetting::MAX_CASCADE_NUM;

					auto geometryInjectionPass = geometryInjectionPassProcessor.Process( meshInfo );
					if ( geometryInjectionPass.has_value() )
					{
						const agl::TextureTrait& rsmTextureTrait = passResouce.m_worldPosition->GetTrait();
						uint32 dimensions[] = { rsmTextureTrait.m_width, rsmTextureTrait.m_height, rsmTextureTrait.m_depth };
						SetShaderValue( commandList, GeometryInjectionVS().RSMsDimensions(), dimensions );

						Vector lightDirection = params.lightInfo->Proxy()->GetDirection();
						SetShaderValue( commandList, GeometryInjectionVS().LightDirection(), lightDirection );

						ResourceBinder resourceBinder;
						resourceBinder.Add( "SceneViewParameters", params.m_viewShaderArguments.Get() );
						resourceBinder.Add( "LPVCommonParameters", m_lpvCommon.Get() );
						resourceBinder.Add( "ShadowDepthPassParameters", params.m_shadowDepthPassParameters.Get() );

						resourceBinder.Add( "RSMsWorldPosition", passResouce.m_worldPosition->SRV() );
						resourceBinder.Add( "RSMsNormal", passResouce.m_normal->SRV() );
						resourceBinder.Add( "RSMsFlux", passResouce.m_flux->SRV() );

						DrawSnapshot& snapshot = *geometryInjectionPass;
						GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
						resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );

						AddSingleDrawPass( commandList, snapshot );
					}
				} );
		}
	}
}
