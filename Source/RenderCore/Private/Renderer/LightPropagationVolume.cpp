#include "LightPropagationVolume.h"

#include "AbstractGraphicsInterface.h"
#include "ComputePipelineState.h"
#include "GlobalShaders.h"
#include "LightProxy.h"
#include "PassProcessor.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/ShadowInfo.h"
#include "SceneRenderer.h"
#include "ShaderParameterUtils.h"
#include "TransitionUtils.h"
#include "VertexCollection.h"

namespace rendercore
{
	class ClearLpvCS final : public GlobalShaderCommon<ComputeShader, ClearLpvCS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	private:
		DEFINE_SHADER_PARAM( CoeffR );
		DEFINE_SHADER_PARAM( CoeffG );
		DEFINE_SHADER_PARAM( CoeffB );
	};

	class DownSampleRSMsCS final : public GlobalShaderCommon<ComputeShader, DownSampleRSMsCS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	private:
		DEFINE_SHADER_PARAM( LPVCommonParameters ); // b2

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
		using GlobalShaderCommon::GlobalShaderCommon;

	private:
		DEFINE_SHADER_PARAM( RSMsDimensions );
	};

	class LightInjectionGS final : public GlobalShaderCommon<GeometryShader, LightInjectionGS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	private:
	};

	class LightInjectionPS final : public GlobalShaderCommon<PixelShader, LightInjectionPS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	private:
	};

	class LightInjectionPassProcessor final : public IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
	};

	std::optional<DrawSnapshot> LightInjectionPassProcessor::Process( const PrimitiveSubMesh& subMesh )
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

	REGISTER_GLOBAL_SHADER( ClearLpvCS, "./Assets/Shaders/IndirectLighting/LPV/CS_ClearLPV.asset" );
	REGISTER_GLOBAL_SHADER( DownSampleRSMsCS, "./Assets/Shaders/IndirectLighting/LPV/CS_DownSampleRSMs.asset" );
	REGISTER_GLOBAL_SHADER( LightInjectionVS, "./Assets/Shaders/IndirectLighting/LPV/VS_LightInjection.asset" );
	REGISTER_GLOBAL_SHADER( LightInjectionGS, "./Assets/Shaders/IndirectLighting/LPV/GS_LightInjection.asset" );
	REGISTER_GLOBAL_SHADER( LightInjectionPS, "./Assets/Shaders/IndirectLighting/LPV/PS_LightInjection.asset" );

	void LightPropagationVolume::Prepare()
	{
		InitResource();
		ClearLPV();
	}

	void LightPropagationVolume::AddLight( const LightInjectionParameters& params )
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

		RSMTextures downSampledTex = DownSampleRSMs( *params.lightInfo, params.m_rsmTextures );
		InjectToLPV( params.m_sceneViewParameters.Get(), params.m_shadowDepthPassParameters.Get(), downSampledTex );
	}

	void LightPropagationVolume::InitResource()
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
			|| m_lpvTextures.m_coeffB.Get() == nullptr )
		{
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

			m_lpvTextures.m_coeffR = agl::Texture::Create( trait, "LPV.Coeff.R" );
			m_lpvTextures.m_coeffR->Init();

			m_lpvTextures.m_coeffG = agl::Texture::Create( trait, "LPV.Coeff.G" );
			m_lpvTextures.m_coeffG->Init();

			m_lpvTextures.m_coeffB = agl::Texture::Create( trait, "LPV.Coeff.B" );
			m_lpvTextures.m_coeffB->Init();
		}
	}

	void LightPropagationVolume::ClearLPV()
	{
		ClearLpvCS clearLpvCS;

		agl::ShaderBindings shaderBindings = CreateShaderBindings( clearLpvCS );
		BindResource( shaderBindings, clearLpvCS.CoeffR(), m_lpvTextures.m_coeffR );
		BindResource( shaderBindings, clearLpvCS.CoeffG(), m_lpvTextures.m_coeffG );
		BindResource( shaderBindings, clearLpvCS.CoeffB(), m_lpvTextures.m_coeffB );

		agl::RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( clearLpvCS );

		auto commandList = GetCommandList();

		{
			agl::ResourceTransition transitions[] = {
				Transition( *m_lpvTextures.m_coeffR, agl::ResourceState::UnorderedAccess ),
				Transition( *m_lpvTextures.m_coeffG, agl::ResourceState::UnorderedAccess ),
				Transition( *m_lpvTextures.m_coeffB, agl::ResourceState::UnorderedAccess ),
			};

			commandList.Transition( std::extent_v<decltype( transitions )>, transitions );
		}

		commandList.BindPipelineState( pso );
		commandList.BindShaderResources( shaderBindings );

		// [numthreads(4, 4, 4)] -> Dispatch( 32 / 4, 32 / 4, 32 / 4 )
		commandList.Dispatch( 8, 8, 8 );
	}

	RSMTextures LightPropagationVolume::DownSampleRSMs( const LightSceneInfo& lightInfo, const RSMTextures& rsmTextures )
	{
		RSMTextures downSampledTex;

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

		{
			agl::ResourceTransition transitions[] = {
				Transition( *rsmTextures.m_worldPosition, agl::ResourceState::NonPixelShaderResource ),
				Transition( *rsmTextures.m_normal, agl::ResourceState::NonPixelShaderResource ),
				Transition( *rsmTextures.m_flux, agl::ResourceState::NonPixelShaderResource ),
				Transition( *downSampledTex.m_worldPosition, agl::ResourceState::UnorderedAccess ),
				Transition( *downSampledTex.m_normal, agl::ResourceState::UnorderedAccess ),
				Transition( *downSampledTex.m_flux, agl::ResourceState::UnorderedAccess ),
			};

			commandList.Transition( std::extent_v<decltype( transitions )>, transitions );
		}

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

	void LightPropagationVolume::InjectToLPV( agl::Buffer* sceneViewParameters,
		agl::Buffer* shadowDepthPassParameters,
		const RSMTextures& downSampledTex )
	{
		LightInjectionPassProcessor lightInjectionPassProcessor;

		PrimitiveSubMesh meshInfo;
		meshInfo.m_count = 512 * 512 * CascadeShadowSetting::MAX_CASCADE_NUM;

		auto result = lightInjectionPassProcessor.Process( meshInfo );
		if ( result.has_value() )
		{
			auto commandList = GetCommandList();

			{
				agl::ResourceTransition transitions[] = {
					Transition( *downSampledTex.m_worldPosition, agl::ResourceState::NonPixelShaderResource ),
					Transition( *downSampledTex.m_normal, agl::ResourceState::NonPixelShaderResource ),
					Transition( *downSampledTex.m_flux, agl::ResourceState::NonPixelShaderResource ),
					Transition( *m_lpvTextures.m_coeffR, agl::ResourceState::RenderTarget ),
					Transition( *m_lpvTextures.m_coeffG, agl::ResourceState::RenderTarget ),
					Transition( *m_lpvTextures.m_coeffB, agl::ResourceState::RenderTarget ),
				};

				commandList.Transition( std::extent_v<decltype( transitions )>, transitions );
			}

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
				.m_right = 32,
				.m_bottom = 32,
				.m_back = 32
			};
			commandList.SetViewports( 1, &viewport );

			RectangleArea<int32> scissorRect = {
				.m_left = 0L,
				.m_top = 0L,
				.m_right = 32,
				.m_bottom = 32
			};
			commandList.SetScissorRects( 3, &scissorRect );

			const agl::TextureTrait& rsmTextureTrait = downSampledTex.m_worldPosition->GetTrait();
			uint32 dimensions[] = { rsmTextureTrait.m_width, rsmTextureTrait.m_height, rsmTextureTrait.m_depth };
			SetShaderValue( commandList, LightInjectionVS().RSMsDimensions(), dimensions );

			RenderingShaderResource shaderResources;
			shaderResources.AddResource( "SceneViewParameters", sceneViewParameters );
			shaderResources.AddResource( "LPVCommonParameters", m_lpvCommon );
			shaderResources.AddResource( "ShadowDepthPassParameters", shadowDepthPassParameters );

			shaderResources.AddResource( "RSMsWorldPosition", downSampledTex.m_worldPosition->SRV() );
			shaderResources.AddResource( "RSMsNormal", downSampledTex.m_normal->SRV() );
			shaderResources.AddResource( "RSMsFlux", downSampledTex.m_flux->SRV() );

			DrawSnapshot& snapshot = *result;
			GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;
			shaderResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );

			AddSingleDrawPass( snapshot );

			{
				agl::ResourceTransition transitions[] = {
					Transition( *m_lpvTextures.m_coeffR, agl::ResourceState::UnorderedAccess ),
					Transition( *m_lpvTextures.m_coeffG, agl::ResourceState::UnorderedAccess ),
					Transition( *m_lpvTextures.m_coeffB, agl::ResourceState::UnorderedAccess ),
				};

				commandList.Transition( std::extent_v<decltype( transitions )>, transitions );
			}
		}
	}
}
