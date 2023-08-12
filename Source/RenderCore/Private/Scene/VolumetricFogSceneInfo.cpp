#include "Scene/VolumetricFogSceneInfo.h"

#include "CommandList.h"
#include "ComputePipelineState.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "Renderer/ForwardLighting.h"
#include "Renderer/RenderView.h"
#include "Scene/Scene.h"
#include "ShaderParameterUtils.h"
#include "TaskScheduler.h"
#include "VolumetricFogProxy.h"

namespace rendercore
{
	class InscatteringCS : public GlobalShaderCommon<ComputeShader, InscatteringCS>
	{
	public:
		InscatteringCS( const StaticShaderSwitches& switches ) : GlobalShaderCommon<ComputeShader, InscatteringCS>( switches ) {}

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

	class AccumulateScatteringCS : public GlobalShaderCommon<ComputeShader, AccumulateScatteringCS>
	{
		DEFINE_SHADER_PARAM( FrustumVolume );
		DEFINE_SHADER_PARAM( AccumulatedVolume );

		DEFINE_SHADER_PARAM( VolumetricFogParameterBuffer );
	};

	REGISTER_GLOBAL_SHADER( InscatteringCS, "./Assets/Shaders/VolumetricFog/CS_Inscattering.asset" );
	REGISTER_GLOBAL_SHADER( AccumulateScatteringCS, "./Assets/Shaders/VolumetricFog/CS_AccumulateScattering.asset" );

	void VolumetricFogSceneInfo::CreateRenderData()
	{
		if ( m_needCreateRenderData )
		{
			CreateVolumeTexture();

			m_needCreateRenderData = false;
		}
	}

	void VolumetricFogSceneInfo::UpdateParameter()
	{
		if ( m_needUpdateParameter )
		{
			VolumetricFogParameter param = {
			   .m_exposure = 0.4f,
			   .m_depthPackExponent = m_volumetricFogProxy->DepthPackExponent(),
			   .m_nearPlaneDist = m_volumetricFogProxy->NearPlaneDist(),
			   .m_farPlaneDist = m_volumetricFogProxy->FarPlaneDist()
			};

			m_volumetricFogParameter.Update( param );

			m_needUpdateParameter = false;
		}
	}

	void VolumetricFogSceneInfo::PrepareFrustumVolume( Scene& scene, RenderView& renderView, RenderThreadFrameData<ShadowInfo>& shadowInfos )
	{
		auto commandList = GetCommandList();

		++m_numTick;

		CalcInscattering( commandList, scene, renderView, shadowInfos );
		AccumulateScattering( commandList );

		commandList.Commit();
		GetInterface<agl::IAgl>()->WaitGPU();
	}

	VolumetricFogSceneInfo::VolumetricFogSceneInfo( VolumetricFogProxy* proxy )
		: m_volumetricFogProxy( proxy )
	{
	}

	void VolumetricFogSceneInfo::CreateVolumeTexture()
	{
		const std::array<uint32, 3>& frustumGridSize = Proxy()->FrustumGridSize();

		agl::TextureTrait frustumVolumeTrait = {
			.m_width = frustumGridSize[0],
			.m_height = frustumGridSize[1],
			.m_depth = frustumGridSize[2],
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R16G16B16A16_FLOAT,
			.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			.m_bindType = agl::ResourceBindType::RandomAccess | agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::Texture3D
		};

		for ( agl::RefHandle<agl::Texture>& frustumVolume : m_frustumVolume )
		{
			frustumVolume = agl::Texture::Create( frustumVolumeTrait );
			EnqueueRenderTask(
				[texture = frustumVolume]()
				{
					texture->Init();
				} );
		}

		m_accumulatedVolume = agl::Texture::Create( frustumVolumeTrait );
		EnqueueRenderTask(
			[texture = m_accumulatedVolume]()
			{
				texture->Init();
			} );
	}

	void VolumetricFogSceneInfo::CalcInscattering( CommandList& commandList, Scene& scene, RenderView& renderView, RenderThreadFrameData<ShadowInfo>& shadowInfos )
	{
		StaticShaderSwitches switches = InscatteringCS::GetSwitches();
		if ( DefaultRenderCore::IsESMsEnabled() )
		{
			switches.On( Name( "EnableESMs" ), 1 );
		}

		InscatteringCS inscatteringCS( switches );

		agl::RefHandle<agl::ComputePipelineState> inscatteringPSO = PrepareComputePipelineState( inscatteringCS );
		commandList.BindPipelineState( inscatteringPSO );

		SetShaderValue( commandList, inscatteringCS.AsymmetryParameterG(), Proxy()->G() );
		SetShaderValue( commandList, inscatteringCS.UniformDensity(), Proxy()->UniformDensity() );
		SetShaderValue( commandList, inscatteringCS.Intensity(), Proxy()->Intensity() );
		SetShaderValue( commandList, inscatteringCS.TemporalAccum(), ( m_numTick == 0 ) ? 0.f : 1.f );
		SetShaderValue( commandList, inscatteringCS.ShadowBias(), Proxy()->ShadowBias() );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( inscatteringCS );
		BindResource( shaderBindings, inscatteringCS.FrustumVolume(), FrustumVolume() );

		SceneViewConstantBuffer& viewConstant = scene.SceneViewConstant();
		BindResource( shaderBindings, inscatteringCS.SceneViewParameters(), viewConstant.Resource() );

		BindResource( shaderBindings, inscatteringCS.ForwardLightConstant(), renderView.m_forwardLighting->m_lightConstant.Resource() );
		BindResource( shaderBindings, inscatteringCS.ForwardLight(), renderView.m_forwardLighting->m_lightBuffer.Resource() );

		BindResource( shaderBindings, inscatteringCS.VolumetricFogParameterBuffer(), GetVolumetricFogParameter().Resource() );

		SamplerOption shadowSamplerOption;
		if ( DefaultRenderCore::IsESMsEnabled() == false )
		{
			shadowSamplerOption.m_filter |= agl::TextureFilter::Comparison;
			shadowSamplerOption.m_addressU = agl::TextureAddressMode::Border;
			shadowSamplerOption.m_addressV = agl::TextureAddressMode::Border;
			shadowSamplerOption.m_addressW = agl::TextureAddressMode::Border;
			shadowSamplerOption.m_comparisonFunc = agl::ComparisonFunc::LessEqual;
		}
		SamplerState shadowSampler = GraphicsInterface().FindOrCreate( shadowSamplerOption );

		BindResource( shaderBindings, inscatteringCS.ShadowSampler(), shadowSampler.Resource() );

		BindResource( shaderBindings, inscatteringCS.HistoryVolume(), HistoryVolume() );

		SamplerOption historySamplerOption;
		SamplerState historySampler = GraphicsInterface().FindOrCreate( historySamplerOption );

		BindResource( shaderBindings, inscatteringCS.HistorySampler(), historySampler.Resource() );

		const std::array<uint32, 3>& frustumGridSize = Proxy()->FrustumGridSize();
		const uint32 threadGroupCount[3] = {
			static_cast<uint32>( std::ceilf( frustumGridSize[0] / 8.f ) ),
			static_cast<uint32>( std::ceilf( frustumGridSize[1] / 8.f ) ),
			static_cast<uint32>( std::ceilf( frustumGridSize[2] / 8.f ) )
		};

		for ( ShadowInfo& shadowInfo : shadowInfos )
		{
			BindResource( shaderBindings, inscatteringCS.ShadowTexture(), shadowInfo.ShadowMap().m_shadowMap );
			BindResource( shaderBindings, inscatteringCS.ShadowDepthPassParameters(), shadowInfo.ConstantBuffer().Resource() );

			if ( DefaultRenderCore::IsESMsEnabled() )
			{
				BindResource( shaderBindings, inscatteringCS.ESMsParameters(), shadowInfo.ESMsConstantBuffer().Resource() );
			}

			commandList.BindShaderResources( shaderBindings );
			commandList.Dispatch( threadGroupCount[0], threadGroupCount[1], threadGroupCount[2] );
		}
	}

	void VolumetricFogSceneInfo::AccumulateScattering( CommandList& commandList )
	{
		AccumulateScatteringCS accumulateScatteringCS;

		agl::RefHandle<agl::ComputePipelineState> accumulateScatteringPSO = PrepareComputePipelineState( accumulateScatteringCS );
		commandList.BindPipelineState( accumulateScatteringPSO );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( accumulateScatteringCS );

		BindResource( shaderBindings, accumulateScatteringCS.FrustumVolume(), FrustumVolume() );
		BindResource( shaderBindings, accumulateScatteringCS.AccumulatedVolume(), AccumulatedVolume() );

		BindResource( shaderBindings, accumulateScatteringCS.VolumetricFogParameterBuffer(), GetVolumetricFogParameter().Resource() );

		const std::array<uint32, 3>& frustumGridSize = Proxy()->FrustumGridSize();
		const uint32 threadGroupCount[2] = {
			static_cast<uint32>( std::ceilf( frustumGridSize[0] / 8.f ) ),
			static_cast<uint32>( std::ceilf( frustumGridSize[1] / 8.f ) )
		};

		commandList.BindShaderResources( shaderBindings );
		commandList.Dispatch( threadGroupCount[0], threadGroupCount[1] );
	}
}
