#include "Scene/VolumetricFogSceneInfo.h"

#include "CommandList.h"
#include "ComputePipelineState.h"
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
		const agl::ShaderParameter& AsymmetryParameterG() const
		{
			return m_asymmetryParameterG;
		}

		const agl::ShaderParameter& UniformDensity() const
		{
			return m_uniformDensity;
		}

		const agl::ShaderParameter& Intensity() const
		{
			return m_intensity;
		}

		const agl::ShaderParameter& TemporalAccum() const
		{
			return m_temporalAccum;
		}

		const agl::ShaderParameter& ShadowBias() const
		{
			return m_shadowBias;
		}

		const agl::ShaderParameter& FrustumVolume() const
		{
			return m_frustumVolume;
		}

		const agl::ShaderParameter& SceneViewParameters() const
		{
			return m_sceneViewParameters;
		}

		const agl::ShaderParameter& ForwardLightConstant() const
		{
			return m_forwardLightConstant;
		}

		const agl::ShaderParameter& ForwardLight() const
		{
			return m_forwardLight;
		}

		const agl::ShaderParameter& ShadowTexture() const
		{
			return m_shadowTexture;
		}

		const agl::ShaderParameter& ShadowSampler() const
		{
			return m_shadowSampler;
		}

		const agl::ShaderParameter& ShadowDepthPassParameters() const
		{
			return m_shadowDepthPassParameters;
		}

		const agl::ShaderParameter& VolumetricFogParameter() const
		{
			return m_volumetricFogParameter;
		}

		const agl::ShaderParameter& HistoryVolume() const
		{
			return m_historyVolume;
		}

		const agl::ShaderParameter& HistorySampler() const
		{
			return m_historySampler;
		}

		InscatteringCS()
		{
			m_asymmetryParameterG.Bind( GetShader()->ParameterMap(), "AsymmetryParameterG" );
			m_uniformDensity.Bind( GetShader()->ParameterMap(), "UniformDensity" );
			m_intensity.Bind( GetShader()->ParameterMap(), "Intensity" );
			m_temporalAccum.Bind( GetShader()->ParameterMap(), "TemporalAccum" );
			m_shadowBias.Bind( GetShader()->ParameterMap(), "ShadowBias" );

			m_frustumVolume.Bind( GetShader()->ParameterMap(), "FrustumVolume" );

			m_sceneViewParameters.Bind( GetShader()->ParameterMap(), "SceneViewParameters" );

			m_forwardLightConstant.Bind( GetShader()->ParameterMap(), "ForwardLightConstant" );
			m_forwardLight.Bind( GetShader()->ParameterMap(), "ForwardLight" );

			m_shadowTexture.Bind( GetShader()->ParameterMap(), "ShadowTexture" );
			m_shadowSampler.Bind( GetShader()->ParameterMap(), "ShadowSampler" );
			m_shadowDepthPassParameters.Bind( GetShader()->ParameterMap(), "ShadowDepthPassParameters" );

			m_volumetricFogParameter.Bind( GetShader()->ParameterMap(), "VolumetricFogParameterBuffer" );

			m_historyVolume.Bind( GetShader()->ParameterMap(), "HistoryVolume" );
			m_historySampler.Bind( GetShader()->ParameterMap(), "HistorySampler" );
		}

	private:
		agl::ShaderParameter m_asymmetryParameterG;
		agl::ShaderParameter m_uniformDensity;
		agl::ShaderParameter m_intensity;
		agl::ShaderParameter m_temporalAccum;
		agl::ShaderParameter m_shadowBias;

		agl::ShaderParameter m_frustumVolume;

		agl::ShaderParameter m_sceneViewParameters;

		agl::ShaderParameter m_forwardLightConstant;
		agl::ShaderParameter m_forwardLight;

		agl::ShaderParameter m_shadowTexture;
		agl::ShaderParameter m_shadowSampler;
		agl::ShaderParameter m_shadowDepthPassParameters;

		agl::ShaderParameter m_volumetricFogParameter;

		agl::ShaderParameter m_historyVolume;
		agl::ShaderParameter m_historySampler;
	};

	class AccumulateScatteringCS : public GlobalShaderCommon<ComputeShader, AccumulateScatteringCS>
	{
	public:
		const agl::ShaderParameter& FrustumVolume() const
		{
			return m_frustumVolume;
		}

		const agl::ShaderParameter& AccumulatedVolume() const
		{
			return m_accumulatedVolume;
		}

		AccumulateScatteringCS()
		{
			m_frustumVolume.Bind( GetShader()->ParameterMap(), "FrustumVolume" );
			m_accumulatedVolume.Bind( GetShader()->ParameterMap(), "AccumulatedVolume" );
		}

	private:
		agl::ShaderParameter m_frustumVolume;
		agl::ShaderParameter m_accumulatedVolume;
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
		InscatteringCS inscatteringCS;

		agl::RefHandle<agl::ComputePipelineState> inscatteringPSO = PrepareComputePipelineState( inscatteringCS.GetShader()->Resource() );
		commandList.BindPipelineState( inscatteringPSO );

		SetShaderValue( commandList, inscatteringCS.AsymmetryParameterG(), Proxy()->G() );
		SetShaderValue( commandList, inscatteringCS.UniformDensity(), Proxy()->UniformDensity() );
		SetShaderValue( commandList, inscatteringCS.Intensity(), Proxy()->Intensity() );
		SetShaderValue( commandList, inscatteringCS.TemporalAccum(), ( m_numTick == 0 ) ? 0.f : 1.f );
		SetShaderValue( commandList, inscatteringCS.ShadowBias(), Proxy()->ShadowBias() );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( inscatteringCS.GetShader() );
		BindResource( shaderBindings, inscatteringCS.FrustumVolume(), FrustumVolume() );

		SceneViewConstantBuffer& viewConstant = scene.SceneViewConstant();
		BindResource( shaderBindings, inscatteringCS.SceneViewParameters(), viewConstant.Resource() );

		BindResource( shaderBindings, inscatteringCS.ForwardLightConstant(), renderView.m_forwardLighting->m_lightConstant.Resource() );
		BindResource( shaderBindings, inscatteringCS.ForwardLight(), renderView.m_forwardLighting->m_lightBuffer.Resource() );

		BindResource( shaderBindings, inscatteringCS.VolumetricFogParameter(), GetVolumetricFogParameter().Resource() );

		SamplerOption shadowSamplerOption;
		shadowSamplerOption.m_filter |= agl::TextureFilter::Comparison;
		shadowSamplerOption.m_addressU = agl::TextureAddressMode::Border;
		shadowSamplerOption.m_addressV = agl::TextureAddressMode::Border;
		shadowSamplerOption.m_addressW = agl::TextureAddressMode::Border;
		shadowSamplerOption.m_comparisonFunc = agl::ComparisonFunc::LessEqual;
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

			commandList.BindShaderResources( shaderBindings );
			commandList.Dispatch( threadGroupCount[0], threadGroupCount[1], threadGroupCount[2] );
		}
	}

	void VolumetricFogSceneInfo::AccumulateScattering( CommandList& commandList )
	{
		AccumulateScatteringCS accumulateScatteringCS;

		agl::RefHandle<agl::ComputePipelineState> accumulateScatteringPSO = PrepareComputePipelineState( accumulateScatteringCS.GetShader()->Resource() );
		commandList.BindPipelineState( accumulateScatteringPSO );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( accumulateScatteringCS.GetShader() );

		BindResource( shaderBindings, accumulateScatteringCS.FrustumVolume(), FrustumVolume() );
		BindResource( shaderBindings, accumulateScatteringCS.AccumulatedVolume(), AccumulatedVolume() );

		const std::array<uint32, 3>& frustumGridSize = Proxy()->FrustumGridSize();
		const uint32 threadGroupCount[2] = {
			static_cast<uint32>( std::ceilf( frustumGridSize[0] / 8.f ) ),
			static_cast<uint32>( std::ceilf( frustumGridSize[1] / 8.f ) )
		};

		commandList.BindShaderResources( shaderBindings );
		commandList.Dispatch( threadGroupCount[0], threadGroupCount[1] );
	}
}
