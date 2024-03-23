#include "LightPropagationVolume.h"

#include "AbstractGraphicsInterface.h"
#include "CommonRenderResource.h"
#include "ComputePipelineState.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "LightProxy.h"
#include "PassProcessor.h"
#include "RenderTargetPool.h"
#include "RenderView.h"
#include "ResourceBarrierUtils.h"
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
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override
		{
			PassShader passShader = {
				.m_vertexShader = LightInjectionVS(),
				.m_geometryShader = LightInjectionGS(),
				.m_pixelShader = LightInjectionPS()
			};

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
	};

	class GeometryInjectionPassProcessor final : public IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override
		{
			PassShader passShader = {
				.m_vertexShader = GeometryInjectionVS(),
				.m_geometryShader = GeometryInjectionGS(),
				.m_pixelShader = GeometryInjectionPS()
			};

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
	};

	class LpvRenderPassProcessor final : public IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override
		{
			PassShader passShader = {
				.m_vertexShader = FullScreenQuadVS(),
				.m_geometryShader = nullptr,
				.m_pixelShader = RenderLpvPS()
			};

			DepthStencilOption RSMsDrawPassDepthOption;
			RSMsDrawPassDepthOption.m_depth.m_enable = false;
			RSMsDrawPassDepthOption.m_depth.m_writeDepth = false;

			PassRenderOption passRenderOption = {
				.m_depthStencilOption = &RSMsDrawPassDepthOption
			};

			return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
		}
	};

	void LightPropagationVolume::Prepare( const RenderViewGroup& renderViewGroup )
	{
		std::pair<uint32, uint32> curRtSize = renderViewGroup.GetViewport().Size();

		InitResource( curRtSize );
		ClearLPV();
	}

	void LightPropagationVolume::InjectLight( const LpvLightInjectionParameters& params )
	{
		bool isValid = params.lightInfo != nullptr
			&& params.m_sceneViewParameters.Get() != nullptr
			&& params.m_shadowDepthPassParameters.Get() != nullptr
			&& params.m_rsmTextures.m_worldPosition != nullptr
			&& params.m_rsmTextures.m_normal != nullptr
			&& params.m_rsmTextures.m_flux != nullptr;

		if ( isValid == false )
		{
			return;
		}

		LpvRSMTextures downSampledTex = DownSampleRSMs( *params.lightInfo, params.m_rsmTextures );
		InjectToLPV( params, downSampledTex );
	}

	void LightPropagationVolume::Propagate()
	{
		LPVTextures tempTextures = AllocVolumeTextures( false );

		auto commandList = GetCommandList();

		commandList.AddTransition( Transition( *m_lpvTextures.m_coeffR, agl::ResourceState::UnorderedAccess ) );
		commandList.AddTransition( Transition( *m_lpvTextures.m_coeffG, agl::ResourceState::UnorderedAccess ) );
		commandList.AddTransition( Transition( *m_lpvTextures.m_coeffB, agl::ResourceState::UnorderedAccess ) );
		commandList.AddTransition( Transition( *m_lpvTextures.m_coeffOcclusion, agl::ResourceState::UnorderedAccess ) );
		commandList.AddTransition( Transition( *tempTextures.m_coeffR, agl::ResourceState::UnorderedAccess ) );
		commandList.AddTransition( Transition( *tempTextures.m_coeffG, agl::ResourceState::UnorderedAccess ) );
		commandList.AddTransition( Transition( *tempTextures.m_coeffB, agl::ResourceState::UnorderedAccess ) );

		LightPropagationCS lightPropagationCS;

		agl::ShaderBindings shaderBindings = CreateShaderBindings( lightPropagationCS );
		BindResource( shaderBindings, lightPropagationCS.LPVCommonParameters(), m_lpvCommon );
		BindResource( shaderBindings, lightPropagationCS.CoeffOcclusion(), m_lpvTextures.m_coeffOcclusion );

		auto blackBorderLinearSampler = StaticSamplerState<agl::TextureFilter::MinMagMipLinear
			, agl::TextureAddressMode::Border
			, agl::TextureAddressMode::Border
			, agl::TextureAddressMode::Border
			, 0.f
			, agl::ComparisonFunc::Never
			, Color( 0, 0, 0, 0 )>::Get();
		BindResource( shaderBindings, lightPropagationCS.BlackBorderLinearSampler(), blackBorderLinearSampler );

		agl::RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( lightPropagationCS );

		// [numthreads(4, 4, 32)] -> Dispatch( 32 / 8, 32 / 8, 32 / 1 )
		for ( uint32 i = 0; i < DefaultRenderCore::NumLpvIteration(); ++i )
		{
			BindResource( shaderBindings, lightPropagationCS.CoeffR(), m_lpvTextures.m_coeffR );
			BindResource( shaderBindings, lightPropagationCS.CoeffG(), m_lpvTextures.m_coeffG );
			BindResource( shaderBindings, lightPropagationCS.CoeffB(), m_lpvTextures.m_coeffB );
			BindResource( shaderBindings, lightPropagationCS.OutCoeffR(), tempTextures.m_coeffR );
			BindResource( shaderBindings, lightPropagationCS.OutCoeffG(), tempTextures.m_coeffG );
			BindResource( shaderBindings, lightPropagationCS.OutCoeffB(), tempTextures.m_coeffB );
			
			SetShaderValue( commandList, lightPropagationCS.InterationCount(), i + 1 );

			commandList.BindPipelineState( pso );
			commandList.BindShaderResources( shaderBindings );
			commandList.Dispatch( 4, 4, 32 );

			std::swap( m_lpvTextures, tempTextures );
		}

		m_lpvTextures = tempTextures;
	}

	void LightPropagationVolume::Render( const LpvRenderingParameters& param, RenderingShaderResource& outRenderingShaderResource )
	{
		LpvRenderPassProcessor lpvDrawPassProcessor;
		auto result = lpvDrawPassProcessor.Process( FullScreenQuadDrawInfo() );
		if ( result.has_value() == false )
		{
			return;
		}

		DrawSnapshot& snapshot = *result;

		auto blackBorderSampler = StaticSamplerState<agl::TextureFilter::MinMagMipLinear
			, agl::TextureAddressMode::Border
			, agl::TextureAddressMode::Border
			, agl::TextureAddressMode::Border
			, 0.f
			, agl::ComparisonFunc::Never
			, Color( 0, 0, 0, 255 )>::Get();

		RenderingShaderResource shaderResources;
		shaderResources.AddResource( "ViewSpaceDistance", param.m_viewSpaceDistance->SRV() );
		shaderResources.AddResource( "WorldNormal", param.m_worldNormal->SRV() );
		shaderResources.AddResource( "CoeffR", m_lpvTextures.m_coeffR->SRV() );
		shaderResources.AddResource( "CoeffG", m_lpvTextures.m_coeffG->SRV() );
		shaderResources.AddResource( "CoeffB", m_lpvTextures.m_coeffB->SRV() );
		shaderResources.AddResource( "BlackBorderSampler", blackBorderSampler.Resource() );
		shaderResources.AddResource( "LPVCommonParameters", m_lpvCommon );

		auto commandList = GetCommandList();

		commandList.AddTransition( Transition( *m_indirectIllumination.Get(), agl::ResourceState::RenderTarget ) );

		agl::RenderTargetView* rtv = m_indirectIllumination->RTV();
		commandList.BindRenderTargets( &rtv, 1, nullptr );

		const agl::TextureTrait& trait = m_indirectIllumination->GetTrait();

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

		// Update invalidated resources
		GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
		outRenderingShaderResource.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

		AddSingleDrawPass( snapshot );

		commandList.AddTransition( Transition( *m_indirectIllumination.Get(), agl::ResourceState::PixelShaderResource ) );

		outRenderingShaderResource.AddResource( "IndirectIllumination", m_indirectIllumination->SRV() );
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

		m_indirectIllumination = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "LPV.Illumination" );
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

		volumeTextures.m_coeffR = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "LPV.Coeff.R" );
		volumeTextures.m_coeffG = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "LPV.Coeff.G" );
		volumeTextures.m_coeffB = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "LPV.Coeff.B" );

		if ( allocForOcclusion )
		{
			volumeTextures.m_coeffOcclusion = RenderTargetPool::GetInstance().FindFreeRenderTarget( trait, "LPV.Coeff.Occlusion" );
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

			auto dest = static_cast<uint8*>( GraphicsInterface().Lock( m_lpvCommon ).m_data );

			Vector4 textureDimension = Vector4( 32.f, 32.f, 32.f, 32.f );
			memcpy( dest, &textureDimension, sizeof( Vector4 ) );
			dest += sizeof( Vector4 );

			Vector4 cellSize = Vector4( 4.f, 4.f, 4.f, 4.f );
			memcpy( dest, &cellSize, sizeof( Vector4 ) );

			GraphicsInterface().UnLock( m_lpvCommon );
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

	void LightPropagationVolume::ClearLPV()
	{
		ClearLpvCS clearLpvCS;

		agl::ShaderBindings shaderBindings = CreateShaderBindings( clearLpvCS );
		BindResource( shaderBindings, clearLpvCS.CoeffR(), m_lpvTextures.m_coeffR );
		BindResource( shaderBindings, clearLpvCS.CoeffG(), m_lpvTextures.m_coeffG );
		BindResource( shaderBindings, clearLpvCS.CoeffB(), m_lpvTextures.m_coeffB );
		BindResource( shaderBindings, clearLpvCS.CoeffOcclusion(), m_lpvTextures.m_coeffOcclusion );

		agl::RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( clearLpvCS );

		auto commandList = GetCommandList();

		commandList.AddTransition( Transition( *m_lpvTextures.m_coeffR, agl::ResourceState::UnorderedAccess ) );
		commandList.AddTransition( Transition( *m_lpvTextures.m_coeffG, agl::ResourceState::UnorderedAccess ) );
		commandList.AddTransition( Transition( *m_lpvTextures.m_coeffB, agl::ResourceState::UnorderedAccess ) );
		commandList.AddTransition( Transition( *m_lpvTextures.m_coeffOcclusion, agl::ResourceState::UnorderedAccess ) );

		commandList.BindPipelineState( pso );
		commandList.BindShaderResources( shaderBindings );

		// [numthreads(8, 8, 8)] -> Dispatch( 32 / 4, 32 / 4, 32 / 4 )
		commandList.Dispatch( 8, 8, 8 );
	}

	LpvRSMTextures LightPropagationVolume::DownSampleRSMs( const LightSceneInfo& lightInfo, const LpvRSMTextures& rsmTextures )
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

		auto commandList = GetCommandList();

		commandList.AddTransition( Transition( *rsmTextures.m_worldPosition, agl::ResourceState::NonPixelShaderResource ) );
		commandList.AddTransition( Transition( *rsmTextures.m_normal, agl::ResourceState::NonPixelShaderResource ) );
		commandList.AddTransition( Transition( *rsmTextures.m_flux, agl::ResourceState::NonPixelShaderResource ) );
		commandList.AddTransition( Transition( *downSampledTex.m_worldPosition, agl::ResourceState::UnorderedAccess ) );
		commandList.AddTransition( Transition( *downSampledTex.m_normal, agl::ResourceState::UnorderedAccess ) );
		commandList.AddTransition( Transition( *downSampledTex.m_flux, agl::ResourceState::UnorderedAccess ) );

		DownSampleRSMsCS downSampleRSMsCS;
		agl::RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( downSampleRSMsCS );

		commandList.BindPipelineState( pso );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( downSampleRSMsCS );
		BindResource( shaderBindings, downSampleRSMsCS.LPVCommonParameters(), m_lpvCommon );
		BindResource( shaderBindings, downSampleRSMsCS.RSMsWorldPosition(), rsmTextures.m_worldPosition );
		BindResource( shaderBindings, downSampleRSMsCS.RSMsNormal(), rsmTextures.m_normal);
		BindResource( shaderBindings, downSampleRSMsCS.RSMsFlux(), rsmTextures.m_flux );
		BindResource( shaderBindings, downSampleRSMsCS.OutRSMsWorldPosition(), downSampledTex.m_worldPosition );
		BindResource( shaderBindings, downSampleRSMsCS.OutRSMsNormal(), downSampledTex.m_normal );
		BindResource( shaderBindings, downSampleRSMsCS.OutRSMsFlux(), downSampledTex.m_flux );

		const agl::TextureTrait& rsmTextureTrait = rsmTextures.m_worldPosition->GetTrait();
		uint32 dimensions[] = { rsmTextureTrait.m_width, rsmTextureTrait.m_height, rsmTextureTrait.m_depth };

		SetShaderValue( commandList, downSampleRSMsCS.RSMsDimensions(), dimensions );

		uint32 kernelSize = rsmTextureTrait.m_width / 512;
		SetShaderValue( commandList, downSampleRSMsCS.KernelSize(), kernelSize );

		Vector toLightDir = -lightInfo.Proxy()->GetLightProperty().m_direction;
		SetShaderValue( commandList, downSampleRSMsCS.ToLightDir(), toLightDir );

		commandList.BindShaderResources( shaderBindings );

		// [numthreads(8, 8, 1)] -> Dispatch( 512 / 8, 512 / 8, CascadeShadowSetting::MAX_CASCADE_NUM / 1 )
		commandList.Dispatch( 64, 64, CascadeShadowSetting::MAX_CASCADE_NUM );

		return downSampledTex;
	}

	void LightPropagationVolume::InjectToLPV( const LpvLightInjectionParameters& params, const LpvRSMTextures& downSampledTex )
	{
		LightInjectionPassProcessor lightInjectionPassProcessor;

		PrimitiveSubMesh meshInfo;
		meshInfo.m_count = 512 * 512 * CascadeShadowSetting::MAX_CASCADE_NUM;

		auto lightInjectionPass = lightInjectionPassProcessor.Process( meshInfo );
		if ( lightInjectionPass.has_value() )
		{
			auto commandList = GetCommandList();

			commandList.AddTransition( Transition( *downSampledTex.m_worldPosition, agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *downSampledTex.m_normal, agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *downSampledTex.m_flux, agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *m_lpvTextures.m_coeffR, agl::ResourceState::RenderTarget ) );
			commandList.AddTransition( Transition( *m_lpvTextures.m_coeffG, agl::ResourceState::RenderTarget ) );
			commandList.AddTransition( Transition( *m_lpvTextures.m_coeffB, agl::ResourceState::RenderTarget ) );

			agl::RenderTargetView* rtvs[] = {
				m_lpvTextures.m_coeffR->RTV(),
				m_lpvTextures.m_coeffG->RTV(),
				m_lpvTextures.m_coeffB->RTV(),
			};

			commandList.BindRenderTargets( rtvs, 3, nullptr );

			CubeArea<float> viewport = {
				.m_left = 0.f,
				.m_top = 0.f,
				.m_front = 0.f,
				.m_right = 32.f,
				.m_bottom = 32.f,
				.m_back = 1.f
			};
			commandList.SetViewports( 1, &viewport );

			RectangleArea<int32> scissorRect = {
				.m_left = 0L,
				.m_top = 0L,
				.m_right = 32,
				.m_bottom = 32
			};
			commandList.SetScissorRects( 1, &scissorRect );

			const agl::TextureTrait& rsmTextureTrait = downSampledTex.m_worldPosition->GetTrait();
			uint32 dimensions[] = { rsmTextureTrait.m_width, rsmTextureTrait.m_height, rsmTextureTrait.m_depth };
			SetShaderValue( commandList, LightInjectionVS().RSMsDimensions(), dimensions );

			Vector4 vSurfelArea[CascadeShadowSetting::MAX_CASCADE_NUM] = {};
			for ( int32 i = 0; i < CascadeShadowSetting::MAX_CASCADE_NUM; ++i )
			{
				float surfelArea = params.m_surfelAreas[i];
				vSurfelArea[i] = Vector4( surfelArea, surfelArea, surfelArea, surfelArea );
			}
			SetShaderValue( commandList, LightInjectionVS().SurfelArea(), vSurfelArea );

			RenderingShaderResource shaderResources;
			shaderResources.AddResource( "SceneViewParameters", params.m_sceneViewParameters );
			shaderResources.AddResource( "LPVCommonParameters", m_lpvCommon );
			shaderResources.AddResource( "ShadowDepthPassParameters", params.m_shadowDepthPassParameters );

			shaderResources.AddResource( "RSMsWorldPosition", downSampledTex.m_worldPosition->SRV() );
			shaderResources.AddResource( "RSMsNormal", downSampledTex.m_normal->SRV() );
			shaderResources.AddResource( "RSMsFlux", downSampledTex.m_flux->SRV() );

			DrawSnapshot& snapshot = *lightInjectionPass;
			GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
			shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

			AddSingleDrawPass( snapshot );
		}

		GeometryInjectionPassProcessor geometryInjectionPassProcessor;
		auto geometryInjectionPass = geometryInjectionPassProcessor.Process( meshInfo );
		if ( geometryInjectionPass.has_value() )
		{
			auto commandList = GetCommandList();

			commandList.AddTransition( Transition( *m_lpvTextures.m_coeffOcclusion, agl::ResourceState::RenderTarget ) );

			agl::RenderTargetView* rtvs[] = {
				m_lpvTextures.m_coeffOcclusion->RTV(),
			};

			commandList.BindRenderTargets( rtvs, 1, nullptr );

			CubeArea<float> viewport = {
				.m_left = 0.f,
				.m_top = 0.f,
				.m_front = 0.f,
				.m_right = 32.f,
				.m_bottom = 32.f,
				.m_back = 1.f
			};
			commandList.SetViewports( 1, &viewport );

			RectangleArea<int32> scissorRect = {
				.m_left = 0L,
				.m_top = 0L,
				.m_right = 32,
				.m_bottom = 32
			};
			commandList.SetScissorRects( 1, &scissorRect );

			const agl::TextureTrait& rsmTextureTrait = downSampledTex.m_worldPosition->GetTrait();
			uint32 dimensions[] = { rsmTextureTrait.m_width, rsmTextureTrait.m_height, rsmTextureTrait.m_depth };
			SetShaderValue( commandList, GeometryInjectionVS().RSMsDimensions(), dimensions );

			Vector lightDirection = params.lightInfo->Proxy()->GetDirection();
			SetShaderValue( commandList, GeometryInjectionVS().LightDirection(), lightDirection );

			RenderingShaderResource shaderResources;
			shaderResources.AddResource( "SceneViewParameters", params.m_sceneViewParameters );
			shaderResources.AddResource( "LPVCommonParameters", m_lpvCommon );
			shaderResources.AddResource( "ShadowDepthPassParameters", params.m_shadowDepthPassParameters );

			shaderResources.AddResource( "RSMsWorldPosition", downSampledTex.m_worldPosition->SRV() );
			shaderResources.AddResource( "RSMsNormal", downSampledTex.m_normal->SRV() );
			shaderResources.AddResource( "RSMsFlux", downSampledTex.m_flux->SRV() );

			DrawSnapshot& snapshot = *geometryInjectionPass;
			GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
			shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

			AddSingleDrawPass( snapshot );
		}
	}
}
