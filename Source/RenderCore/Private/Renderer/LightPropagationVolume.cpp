#include "LightPropagationVolume.h"

#include "AbstractGraphicsInterface.h"
#include "GlobalShaders.h"
#include "LightProxy.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/ShadowInfo.h"
#include "ShaderParameterUtils.h"
#include "TransitionUtils.h"
#include "ComputePipelineState.h"

namespace rendercore
{
	class DownSampleRSMsCS final : public GlobalShaderCommon<ComputeShader, DownSampleRSMsCS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	private:
		DEFINE_SHADER_PARAM( LPVCommonParameters );

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

	REGISTER_GLOBAL_SHADER( DownSampleRSMsCS, "./Assets/Shaders/IndirectLighting/LPV/CS_DownSampleRSMs.asset" );

	void LightPropagationVolume::AddLight( const LightSceneInfo& lightInfo, const RSMTextures& rsmTextures )
	{
		bool isRSMTextureValid = rsmTextures.m_worldPosition != nullptr
			&& rsmTextures.m_normal != nullptr
			&& rsmTextures.m_flux != nullptr;

		if ( isRSMTextureValid == false )
		{
			return;
		}

		InitResource();
		RSMTextures downSampledTex = DownSampleRSMs( lightInfo, rsmTextures );
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

		{
			agl::ResourceTransition transitions[] = {
				Transition( *downSampledTex.m_worldPosition, agl::ResourceState::NonPixelShaderResource ),
				Transition( *downSampledTex.m_normal, agl::ResourceState::NonPixelShaderResource ),
				Transition( *downSampledTex.m_flux, agl::ResourceState::NonPixelShaderResource ),
			};

			commandList.Transition( std::extent_v<decltype( transitions )>, transitions );
		}

		// [numthreads(8, 8, 1)] -> Dispatch( 512 / 8, 512 / 8, CascadeShadowSetting::MAX_CASCADE_NUM / 1 )
		commandList.Dispatch( 64, 64, CascadeShadowSetting::MAX_CASCADE_NUM );

		return downSampledTex;
	}
}
