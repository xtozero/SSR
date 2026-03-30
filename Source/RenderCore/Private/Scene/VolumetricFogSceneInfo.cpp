#include "Scene/VolumetricFogSceneInfo.h"

#include "CommandList.h"
#include "ComputePipelineState.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "Renderer/ForwardLighting.h"
#include "Renderer/RenderView.h"
#include "RenderGraph.h"
#include "Scene/Scene.h"
#include "ShaderParameterUtils.h"
#include "StaticState.h"
#include "TaskScheduler.h"
#include "VolumetricFogProxy.h"

namespace rendercore
{
	class InscatteringCS final : public GlobalShaderBase<ComputeShader, InscatteringCS>
	{
		using GlobalShaderBase::GlobalShaderBase;

	private:
		DEFINE_SHADER_PARAM( AsymmetryParameterG );
		DEFINE_SHADER_PARAM( UniformDensity );
		DEFINE_SHADER_PARAM( Intensity );
		DEFINE_SHADER_PARAM( TemporalAccum );
		DEFINE_SHADER_PARAM( ShadowBias );

		DEFINE_SHADER_PARAM( FrustumVolume );

		DEFINE_SHADER_PARAM( SceneViewParameters );

		DEFINE_SHADER_PARAM( ForwardLightConstant );
		DEFINE_SHADER_PARAM( ForwardLight );

		DEFINE_SHADER_PARAM( ShadowTexture );
		DEFINE_SHADER_PARAM( ShadowSampler );
		DEFINE_SHADER_PARAM( ShadowDepthPassParameters );
		DEFINE_SHADER_PARAM( ESMsParameters );

		DEFINE_SHADER_PARAM( VolumetricFogParameterBuffer );

		DEFINE_SHADER_PARAM( HistoryVolume );
		DEFINE_SHADER_PARAM( HistorySampler );
	};

	class AccumulateScatteringCS final : public GlobalShaderBase<ComputeShader, AccumulateScatteringCS>
	{
		DEFINE_SHADER_PARAM( FrustumVolume );
		DEFINE_SHADER_PARAM( AccumulatedVolume );

		DEFINE_SHADER_PARAM( VolumetricFogParameterBuffer );
	};

	REGISTER_GLOBAL_SHADER( InscatteringCS, "VolumetricFog/CS_Inscattering.fx", "main" );
	REGISTER_GLOBAL_SHADER( AccumulateScatteringCS, "VolumetricFog/CS_AccumulateScattering.fx", "main" );

	void VolumetricFogSceneInfo::CreateRenderData()
	{
		if ( m_needCreateRenderData )
		{
			CreateVolumeTexture();

			m_needCreateRenderData = false;
		}
	}

	void VolumetricFogSceneInfo::PrepareFrustumVolume( RenderGraph& renderGraph, Scene& scene, ForwardLightingResource& lightingResource, RenderFrameArray<ShadowInfo>& shadowInfos )
	{
		UpdateParameter();

		++m_numTick;

		auto rgFrustumVolume = renderGraph.RegisterExternalResource( FrustumVolume().Get() );
		auto rgHistoryVolume = renderGraph.RegisterExternalResource( HistoryVolume().Get() );

		BEGIN_RG_RESOURCE_STRUCT( InscatterPassResource )
			DECLARE_RG_TEXTURE_UAV( frustumVolume )
			DECLARE_RG_TEXTURE_PIXEL_SRV( historyVolume )
		END_RG_RESOURCE_STRUCT();

		InscatterPassResource inscatterPassResource = {
			.m_frustumVolume = rgFrustumVolume,
			.m_historyVolume = rgHistoryVolume
		};

		renderGraph.AddPass(
			inscatterPassResource,
			[this, &scene, &lightingResource, &shadowInfos]( ComputeCommandList& commandList )
			{
				CalcInscattering( commandList, scene, lightingResource, shadowInfos );
			} );

		auto rgAccumulatedVolume = renderGraph.RegisterExternalResource( AccumulatedVolume().Get() );

		BEGIN_RG_RESOURCE_STRUCT( AccumulatePassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( frustumVolume )
			DECLARE_RG_TEXTURE_UAV( accumulatedVolume )
		END_RG_RESOURCE_STRUCT();

		AccumulatePassResource accumulatePassResource = {
			.m_frustumVolume = rgFrustumVolume,
			.m_accumulatedVolume = rgAccumulatedVolume
		};

		renderGraph.AddPass(
			accumulatePassResource,
			[this]( ComputeCommandList& commandList )
			{
				AccumulateScattering( commandList );
			}
		);
	}

	VolumetricFogSceneInfo::VolumetricFogSceneInfo( VolumetricFogProxy* proxy )
		: m_volumetricFogProxy( proxy )
	{
		m_shaderArguments = VolumetricFogParameters::CreateShaderArguments();
	}

	void VolumetricFogSceneInfo::CreateVolumeTexture()
	{
		const std::array<uint32, 3>& frustumGridSize = Proxy()->FrustumGridSize();

		agl::TextureDesc frustumVolumeDesc = {
			.m_width = frustumGridSize[0],
			.m_height = frustumGridSize[1],
			.m_depth = frustumGridSize[2],
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R16G16B16A16_FLOAT,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::RandomAccess | agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::Texture3D
		};

		for ( RefHandle<agl::Texture>& frustumVolume : m_frustumVolume )
		{
			frustumVolume = agl::Texture::Create( frustumVolumeDesc, "VolumetricFog.Frustum" );
		}

		m_accumulatedVolume = agl::Texture::Create( frustumVolumeDesc, "VolumetricFog.Accumulated" );
	}

	void VolumetricFogSceneInfo::UpdateParameter()
	{
		if ( m_needUpdateParameter )
		{
			VolumetricFogParameters params = {
			   .Exposure = 0.4f,
			   .DepthPackExponent = m_volumetricFogProxy->DepthPackExponent(),
			   .NearPlaneDist = m_volumetricFogProxy->NearPlaneDist(),
			   .FarPlaneDist = m_volumetricFogProxy->FarPlaneDist()
			};

			m_shaderArguments->Update( params );

			m_needUpdateParameter = false;
		}
	}

	void VolumetricFogSceneInfo::CalcInscattering( ComputeCommandList& commandList, Scene& scene, ForwardLightingResource& lightingResource, RenderFrameArray<ShadowInfo>& shadowInfos )
	{
		StaticShaderSwitches switches = InscatteringCS::GetSwitches();
		if ( DefaultRenderCore::IsESMsEnabled() )
		{
			switches.On( StaticName( "EnableESMs" ), 1 );
		}

		InscatteringCS inscatteringCS( switches );

		RefHandle<agl::ComputePipelineState> inscatteringPSO = PrepareComputePipelineState( inscatteringCS );
		commandList.BindPipelineState( inscatteringPSO.Get() );

		SetShaderValue( commandList, inscatteringCS.AsymmetryParameterG(), Proxy()->G() );
		SetShaderValue( commandList, inscatteringCS.UniformDensity(), Proxy()->UniformDensity() );
		SetShaderValue( commandList, inscatteringCS.Intensity(), Proxy()->Intensity() );
		SetShaderValue( commandList, inscatteringCS.TemporalAccum(), ( m_numTick == 0 ) ? 0.f : 1.f );
		SetShaderValue( commandList, inscatteringCS.ShadowBias(), Proxy()->ShadowBias() );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( inscatteringCS );
		BindResource( shaderBindings, inscatteringCS.FrustumVolume(), FrustumVolume().Get() );
		BindResource( shaderBindings, inscatteringCS.SceneViewParameters(), scene.GetViewShaderArguments().Resource() );
		BindResource( shaderBindings, inscatteringCS.ForwardLightConstant(), lightingResource.m_shaderArguments->Resource() );
		BindResource( shaderBindings, inscatteringCS.ForwardLight(), lightingResource.m_lightBuffer.Resource() );
		BindResource( shaderBindings, inscatteringCS.VolumetricFogParameterBuffer(), GetShaderArguments().Resource() );

		SamplerState shadowSampler;
		if ( DefaultRenderCore::IsESMsEnabled() == false )
		{
			shadowSampler = StaticSamplerState<agl::TextureFilter::MinMagMipLinear | agl::TextureFilter::Comparison
				, agl::TextureAddressMode::Border
				, agl::TextureAddressMode::Border
				, agl::TextureAddressMode::Border
				, 0.f
				, agl::ComparisonFunc::Less>::Get();
		}
		else
		{
			shadowSampler = StaticSamplerState<>::Get();
		}

		BindResource( shaderBindings, inscatteringCS.ShadowSampler(), shadowSampler );

		BindResource( shaderBindings, inscatteringCS.HistoryVolume(), HistoryVolume().Get() );

		SamplerState historySampler = StaticSamplerState<>::Get();

		BindResource( shaderBindings, inscatteringCS.HistorySampler(), historySampler );

		const std::array<uint32, 3>& frustumGridSize = Proxy()->FrustumGridSize();
		const uint32 threadGroupCount[3] = {
			static_cast<uint32>( std::ceilf( frustumGridSize[0] / 8.f ) ),
			static_cast<uint32>( std::ceilf( frustumGridSize[1] / 8.f ) ),
			static_cast<uint32>( std::ceilf( frustumGridSize[2] / 8.f ) )
		};

		for ( ShadowInfo& shadowInfo : shadowInfos )
		{
			BindResource( shaderBindings, inscatteringCS.ShadowTexture(), shadowInfo.ShadowMap().m_shadowMaps[0].Get() );
			BindResource( shaderBindings, inscatteringCS.ShadowDepthPassParameters(), shadowInfo.GetShadowShaderArguments().Resource() );

			if ( DefaultRenderCore::IsESMsEnabled() )
			{
				BindResource( shaderBindings, inscatteringCS.ESMsParameters(), shadowInfo.GetESMsShaderArguments().Resource() );
			}

			commandList.BindShaderResources( shaderBindings );
			commandList.Dispatch( threadGroupCount[0], threadGroupCount[1], threadGroupCount[2] );
		}
	}

	void VolumetricFogSceneInfo::AccumulateScattering( ComputeCommandList& commandList )
	{
		AccumulateScatteringCS accumulateScatteringCS;

		RefHandle<agl::ComputePipelineState> accumulateScatteringPSO = PrepareComputePipelineState( accumulateScatteringCS );
		commandList.BindPipelineState( accumulateScatteringPSO.Get() );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( accumulateScatteringCS );

		BindResource( shaderBindings, accumulateScatteringCS.FrustumVolume(), FrustumVolume().Get() );
		BindResource( shaderBindings, accumulateScatteringCS.AccumulatedVolume(), AccumulatedVolume().Get() );

		BindResource( shaderBindings, accumulateScatteringCS.VolumetricFogParameterBuffer(), GetShaderArguments().Resource() );

		const std::array<uint32, 3>& frustumGridSize = Proxy()->FrustumGridSize();
		const uint32 threadGroupCount[2] = {
			static_cast<uint32>( std::ceilf( frustumGridSize[0] / 8.f ) ),
			static_cast<uint32>( std::ceilf( frustumGridSize[1] / 8.f ) )
		};

		commandList.BindShaderResources( shaderBindings );
		commandList.Dispatch( threadGroupCount[0], threadGroupCount[1] );
	}
}
