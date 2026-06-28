#include "SkyAtmosphereRendering.h"

#include "AbstractGraphicsInterface.h"
#include "AtmosphereConstant.h"
#include "CommandList.h"
#include "CommonRenderResource.h"
#include "ComputePipelineState.h"
#include "GlobalShaders.h"
#include "GpuProfiler.h"
#include "RenderGraph.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"
#include "ShaderParameterUtils.h"
#include "StaticState.h"
#include "VertexCollection.h"

namespace rendercore
{
	class TransmittanceCS final : public GlobalShaderBase<ComputeShader, TransmittanceCS>
	{
		DEFINE_SHADER_PARAM( Transmittance );
	};

	class IrradianceOneCS final : public GlobalShaderBase<ComputeShader, IrradianceOneCS>
	{
		DEFINE_SHADER_PARAM( TransmittanceLut );
		DEFINE_SHADER_PARAM( TransmittanceLutSampler );
		DEFINE_SHADER_PARAM( DeltaE );
	};

	class InscatterOneCS final : public GlobalShaderBase<ComputeShader, InscatterOneCS>
	{
		DEFINE_SHADER_PARAM( TransmittanceLut );
		DEFINE_SHADER_PARAM( TransmittanceLutSampler );
		DEFINE_SHADER_PARAM( DeltaSR );
		DEFINE_SHADER_PARAM( DeltaSM );
	};

	class CopyInscatterOneCS final : public GlobalShaderBase<ComputeShader, CopyInscatterOneCS>
	{
		DEFINE_SHADER_PARAM( DeltaSRLut );
		DEFINE_SHADER_PARAM( DeltaSRLutSampler );
		DEFINE_SHADER_PARAM( DeltaSMLut );
		DEFINE_SHADER_PARAM( DeltaSMLutSampler );
		DEFINE_SHADER_PARAM( Inscatter );
	};

	class InscatterSCS final : public GlobalShaderBase<ComputeShader, InscatterSCS>
	{
		DEFINE_SHADER_PARAM( TransmittanceLut );
		DEFINE_SHADER_PARAM( TransmittanceLutSampler );
		DEFINE_SHADER_PARAM( DeltaELut );
		DEFINE_SHADER_PARAM( DeltaELutSampler );
		DEFINE_SHADER_PARAM( DeltaSRLut );
		DEFINE_SHADER_PARAM( DeltaSRLutSampler );
		DEFINE_SHADER_PARAM( DeltaSMLut );
		DEFINE_SHADER_PARAM( DeltaSMLutSampler );
		DEFINE_SHADER_PARAM( Order );
		DEFINE_SHADER_PARAM( ThreadGroupZ );
		DEFINE_SHADER_PARAM( DeltaJ );
	};

	class IrradianceNCS final : public GlobalShaderBase<ComputeShader, IrradianceNCS>
	{
		DEFINE_SHADER_PARAM( DeltaSRLut );
		DEFINE_SHADER_PARAM( DeltaSRLutSampler );
		DEFINE_SHADER_PARAM( DeltaSMLut );
		DEFINE_SHADER_PARAM( DeltaSMLutSampler );
		DEFINE_SHADER_PARAM( Order );
		DEFINE_SHADER_PARAM( Irradiance );
	};

	class InscatterNCS final : public GlobalShaderBase<ComputeShader, InscatterNCS>
	{
		DEFINE_SHADER_PARAM( TransmittanceLut );
		DEFINE_SHADER_PARAM( TransmittanceLutSampler );
		DEFINE_SHADER_PARAM( DeltaJLut );
		DEFINE_SHADER_PARAM( DeltaJLutSampler );
		DEFINE_SHADER_PARAM( DeltaSR );
	};

	class CopyIrradianceCS final : public GlobalShaderBase<ComputeShader, CopyIrradianceCS>
	{
		DEFINE_SHADER_PARAM( DeltaELut );
		DEFINE_SHADER_PARAM( DeltaELutSampler );
		DEFINE_SHADER_PARAM( Irradiance );
	};

	class CopyInscatterNCS final : public GlobalShaderBase<ComputeShader, CopyInscatterNCS>
	{
		DEFINE_SHADER_PARAM( DeltaSRLut );
		DEFINE_SHADER_PARAM( DeltaSRLutSampler );
		DEFINE_SHADER_PARAM( Inscatter );
	};

	class DrawAtmospherePS final : public GlobalShaderBase<PixelShader, DrawAtmospherePS>
	{};

	REGISTER_GLOBAL_SHADER( TransmittanceCS, "Atmosphere/CS_Transmittance.fx", "main" );
	REGISTER_GLOBAL_SHADER( IrradianceOneCS, "Atmosphere/CS_Irradiance1.fx", "main" );
	REGISTER_GLOBAL_SHADER( InscatterOneCS, "Atmosphere/CS_Inscatter1.fx", "main" );
	REGISTER_GLOBAL_SHADER( CopyInscatterOneCS, "Atmosphere/CS_CopyInscatter1.fx", "main" );
	REGISTER_GLOBAL_SHADER( InscatterSCS, "Atmosphere/CS_InscatterS.fx", "main" );
	REGISTER_GLOBAL_SHADER( IrradianceNCS, "Atmosphere/CS_IrradianceN.fx", "main" );
	REGISTER_GLOBAL_SHADER( InscatterNCS, "Atmosphere/CS_InscatterN.fx", "main" );
	REGISTER_GLOBAL_SHADER( CopyIrradianceCS, "Atmosphere/CS_CopyIrradiance.fx", "main" );
	REGISTER_GLOBAL_SHADER( CopyInscatterNCS, "Atmosphere/CS_CopyInscatterN.fx", "main" );
	REGISTER_GLOBAL_SHADER( DrawAtmospherePS, "Atmosphere/PS_DrawAtmosphere.fx", "main" );

	std::optional<DrawSnapshot> SkyAtmosphereDrawPassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
		BlendOption skyAtmosphereDrawPassBlendOption;
		RenderTargetBlendOption& rt0BlendOption = skyAtmosphereDrawPassBlendOption.m_renderTarget[0];
		rt0BlendOption.m_blendEnable = true;
		rt0BlendOption.m_srcBlend = agl::Blend::One;
		rt0BlendOption.m_destBlend = agl::Blend::One;
		rt0BlendOption.m_srcBlendAlpha = agl::Blend::Zero;
		rt0BlendOption.m_destBlendAlpha = agl::Blend::One;

		DepthStencilOption skyAtmosphereDrawPassDepthOption;
		skyAtmosphereDrawPassDepthOption.m_depth.m_writeDepth = false;

		PassRenderOption passRenderOption = {
			.m_blendOption = &skyAtmosphereDrawPassBlendOption,
			.m_depthStencilOption = &skyAtmosphereDrawPassDepthOption
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
	}

	PassShader SkyAtmosphereDrawPassProcessor::CollectPassShader( [[maybe_unused]] MaterialResource& material ) const
	{
		PassShader passShader = {
			.m_vertexShader = FullScreenQuadVS(),
			.m_pixelShader = DrawAtmospherePS()
		};

		return passShader;
	}

	void InitAtmosphereForScene( Scene& scene )
	{
		SkyAtmosphereRenderSceneInfo& info = *scene.SkyAtmosphereSceneInfo();

		agl::TextureDesc transmittanceLut = {
			.m_width = TRANSMITTANCE_W,
			.m_height = TRANSMITTANCE_H,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::None,
		};

		info.GetTransmittanceLutTexture() = agl::Texture::Create( transmittanceLut, "Atmosphere.TransmittanceLUT" );

		agl::TextureDesc irradianceLut = {
			.m_width = IRRADIANCE_W,
			.m_height = IRRADIANCE_H,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None,
		};

		info.GetIrradianceLutTexture() = agl::Texture::Create( irradianceLut, "Atmosphere.IrradianceLUT" );

		agl::TextureDesc inscatterLut = {
			.m_width = RES_MU_S * RES_NU,
			.m_height = RES_MU,
			.m_depth = RES_R,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::Texture3D,
		};

		info.GetInscatterLutTexture() = agl::Texture::Create( inscatterLut, "Atmosphere.InscatterLUT" );

		info.GetShaderArguments() = SkyAtmosphereRenderParameters::CreateShaderArguments();
	}

	void RenderAtmosphereLookUpTables( RenderGraph& renderGraph, Scene& scene )
	{
		if ( scene.SkyAtmosphereSceneInfo() == nullptr )
		{
			return;
		}

		SkyAtmosphereRenderSceneInfo& info = *scene.SkyAtmosphereSceneInfo();
		if ( info.RebuildLookUpTables() == false )
		{
			return;
		}

		GPU_PROFILE_EVENT( renderGraph, AtomosphereLookUp );

		// 1. Transmittance Table
		auto rgTransmittanceLut = renderGraph.RegisterExternalResource( info.GetTransmittanceLutTexture().Get() );

		BEGIN_RG_RESOURCE_STRUCT( TransmittancePassResource )
			DECLARE_RG_TEXTURE_UAV( transmittanceLut )
		END_RG_RESOURCE_STRUCT();

		TransmittancePassResource transmittancePassResource = {
			.m_transmittanceLut = rgTransmittanceLut
		};

		renderGraph.AddPass(
			transmittancePassResource,
			[transmittancePassResource]( ComputeCommandList& commandList )
			{
				TransmittanceCS transmittanceCS;

				RefHandle<agl::ComputePipelineState> transmittancePSO = PrepareComputePipelineState( transmittanceCS );
				commandList.BindPipelineState( transmittancePSO.Get() );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( transmittanceCS );
				BindResource( shaderBindings, transmittanceCS.Transmittance(), transmittancePassResource.m_transmittanceLut->Get() );

				commandList.BindShaderResources( shaderBindings );
				commandList.Dispatch( TRANSMITTANCE_GROUP_X, TRANSMITTANCE_GROUP_Y );
			} );

		// 2. Ground irradiance due to direct sunlight
		agl::TextureDesc deltaE = {
			.m_width = IRRADIANCE_W,
			.m_height = IRRADIANCE_H,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::None
		};

		auto rgDeltaE = renderGraph.CreateTexture( deltaE, "Atmosphere.DeltaE" );

		BEGIN_RG_RESOURCE_STRUCT( GroundIrrdiancePassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( transmittanceLut )
			DECLARE_RG_TEXTURE_UAV( deltaE )
		END_RG_RESOURCE_STRUCT();

		GroundIrrdiancePassResource groundIrrdiancePassResource = {
			.m_transmittanceLut = rgTransmittanceLut,
			.m_deltaE = rgDeltaE
		};

		renderGraph.AddPass(
			groundIrrdiancePassResource,
			[groundIrrdiancePassResource]( ComputeCommandList& commandList )
			{
				IrradianceOneCS irradianceOneCS;
				RefHandle<agl::ComputePipelineState> irradianceOnePSO = PrepareComputePipelineState( irradianceOneCS );
				commandList.BindPipelineState( irradianceOnePSO.Get() );

				SamplerState pointSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

				agl::ShaderBindings shaderBindings = CreateShaderBindings( irradianceOneCS );
				BindResource( shaderBindings, irradianceOneCS.TransmittanceLut(), groundIrrdiancePassResource.m_transmittanceLut->Get() );
				BindResource( shaderBindings, irradianceOneCS.TransmittanceLutSampler(), pointSampler );
				BindResource( shaderBindings, irradianceOneCS.DeltaE(), groundIrrdiancePassResource.m_deltaE->Get() );

				commandList.BindShaderResources( shaderBindings );
				commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );
			} );

		// 3. Compute single scattering texture deltaS
		agl::TextureDesc deltaS = {
			.m_width = RES_MU_S * RES_NU,
			.m_height = RES_MU,
			.m_depth = RES_R,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::Texture3D
		};

		auto rgDeltaSR = renderGraph.CreateTexture( deltaS, "Atmosphere.DeltaSR" );
		auto rgDeltaSM = renderGraph.CreateTexture( deltaS, "Atmosphere.DeltaSM" );

		BEGIN_RG_RESOURCE_STRUCT( SingleScatteringPassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( transmittanceLut )
			DECLARE_RG_TEXTURE_UAV( deltaSR )
			DECLARE_RG_TEXTURE_UAV( deltaSM )
		END_RG_RESOURCE_STRUCT();

		SingleScatteringPassResource singleScatteringPassResource = {
			.m_transmittanceLut = rgTransmittanceLut,
			.m_deltaSR = rgDeltaSR,
			.m_deltaSM = rgDeltaSM
		};

		renderGraph.AddPass(
			singleScatteringPassResource,
			[singleScatteringPassResource]( ComputeCommandList& commandList )
			{
				InscatterOneCS inscatterOneCS;
				RefHandle<agl::ComputePipelineState> inscatterOnePSO = PrepareComputePipelineState( inscatterOneCS );
				commandList.BindPipelineState( inscatterOnePSO.Get() );

				SamplerState pointSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

				agl::ShaderBindings shaderBindings = CreateShaderBindings( inscatterOneCS );
				BindResource( shaderBindings, inscatterOneCS.TransmittanceLut(), singleScatteringPassResource.m_transmittanceLut->Get() );
				BindResource( shaderBindings, inscatterOneCS.TransmittanceLutSampler(), pointSampler );
				BindResource( shaderBindings, inscatterOneCS.DeltaSR(), singleScatteringPassResource.m_deltaSR->Get() );
				BindResource( shaderBindings, inscatterOneCS.DeltaSM(), singleScatteringPassResource.m_deltaSM->Get() );

				commandList.BindShaderResources( shaderBindings );
				commandList.Dispatch( INSCATTER1_GROUP_X, INSCATTER1_GROUP_Y, INSCATTER1_GROUP_Z );
			} );

		// 4. Copy deltaS into inscatter texture S
		agl::BufferDesc inscatter = {
			.m_stride = sizeof( Vector4 ),
			.m_count = RES_MU_S * RES_NU * RES_MU * RES_R,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::BufferStructured,
			.m_format = agl::ResourceFormat::Unknown
		};

		auto rgInscatter = renderGraph.CreateBuffer( inscatter, "Atmosphere.Inscatter" );

		BEGIN_RG_RESOURCE_STRUCT( CopyDeltaSPassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( deltaSR )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( deltaSM )
			DECLARE_RG_BUFFER_UAV( inscatter )
		END_RG_RESOURCE_STRUCT();

		CopyDeltaSPassResource copyDeltaSPassResource = {
			.m_deltaSR = rgDeltaSR,
			.m_deltaSM = rgDeltaSM,
			.m_inscatter = rgInscatter
		};

		renderGraph.AddPass(
			copyDeltaSPassResource,
			[copyDeltaSPassResource]( ComputeCommandList& commandList )
			{
				CopyInscatterOneCS copyInscatterOneCS;
				RefHandle<agl::ComputePipelineState> copyInscatterOnePSO = PrepareComputePipelineState( copyInscatterOneCS );
				commandList.BindPipelineState( copyInscatterOnePSO.Get() );

				SamplerState pointSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

				agl::ShaderBindings shaderBindings = CreateShaderBindings( copyInscatterOneCS );
				BindResource( shaderBindings, copyInscatterOneCS.DeltaSRLut(), copyDeltaSPassResource.m_deltaSR->Get() );
				BindResource( shaderBindings, copyInscatterOneCS.DeltaSRLutSampler(), pointSampler );
				BindResource( shaderBindings, copyInscatterOneCS.DeltaSMLut(), copyDeltaSPassResource.m_deltaSM->Get() );
				BindResource( shaderBindings, copyInscatterOneCS.DeltaSMLutSampler(), pointSampler );
				BindResource( shaderBindings, copyInscatterOneCS.Inscatter(), copyDeltaSPassResource.m_inscatter->Get() );

				commandList.BindShaderResources( shaderBindings );
				commandList.Dispatch( INSCATTER1_GROUP_X, INSCATTER1_GROUP_Y, INSCATTER1_GROUP_Z );
			} );

		// Irradiance
		auto rgDeltaJ = renderGraph.CreateTexture( deltaS, "Atmosphere.DeltaJ" );

		agl::BufferDesc irradiance = {
			.m_stride = sizeof( Vector4 ),
			.m_count = IRRADIANCE_W * IRRADIANCE_H,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::BufferStructured,
			.m_format = agl::ResourceFormat::Unknown
		};

		Vector4 irradianceData[IRRADIANCE_W * IRRADIANCE_H] = {};
		RefHandle<agl::Buffer> irradianceBuffer = agl::Buffer::Create( irradiance, "Atmosphere.Irradiance", irradianceData );

		auto rgIrradiance = renderGraph.RegisterExternalResource( irradianceBuffer.Get() );

		// loop for each scattering order
		for ( uint32 order = 2; order <= 4; ++order )
		{
			// Compute deltaJ
			BEGIN_RG_RESOURCE_STRUCT( DeltaJPassResource )
				DECLARE_RG_TEXTURE_NONPIXEL_SRV( transmittanceLut )
				DECLARE_RG_TEXTURE_NONPIXEL_SRV( deltaE )
				DECLARE_RG_TEXTURE_NONPIXEL_SRV( deltaSR )
				DECLARE_RG_TEXTURE_NONPIXEL_SRV( deltaSM )
				DECLARE_RG_TEXTURE_UAV( deltaJ )
			END_RG_RESOURCE_STRUCT();

			DeltaJPassResource deltaJPassResource = {
				.m_transmittanceLut = rgTransmittanceLut,
				.m_deltaE = rgDeltaE,
				.m_deltaSR = rgDeltaSR,
				.m_deltaSM = rgDeltaSM,
				.m_deltaJ = rgDeltaJ
			};

			renderGraph.AddPass(
				deltaJPassResource,
				[deltaJPassResource, order]( ComputeCommandList& commandList )
				{
					InscatterSCS inscatterSCS;
					RefHandle<agl::ComputePipelineState> inscatterSPSO = PrepareComputePipelineState( inscatterSCS );

					SamplerState pointSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

					agl::ShaderBindings shaderBindings = CreateShaderBindings( inscatterSCS );
					BindResource( shaderBindings, inscatterSCS.TransmittanceLut(), deltaJPassResource.m_transmittanceLut->Get() );
					BindResource( shaderBindings, inscatterSCS.TransmittanceLutSampler(), pointSampler );
					BindResource( shaderBindings, inscatterSCS.DeltaELut(), deltaJPassResource.m_deltaE->Get() );
					BindResource( shaderBindings, inscatterSCS.DeltaELutSampler(), pointSampler );
					BindResource( shaderBindings, inscatterSCS.DeltaSRLut(), deltaJPassResource.m_deltaSR->Get() );
					BindResource( shaderBindings, inscatterSCS.DeltaSRLutSampler(), pointSampler );
					BindResource( shaderBindings, inscatterSCS.DeltaSMLut(), deltaJPassResource.m_deltaSM->Get() );
					BindResource( shaderBindings, inscatterSCS.DeltaSMLutSampler(), pointSampler );
					BindResource( shaderBindings, inscatterSCS.DeltaJ(), deltaJPassResource.m_deltaJ->Get() );

					for ( uint32 i = 0; i < INSCATTERS_GROUP_Z; ++i )
					{
						SetShaderValue( commandList, inscatterSCS.Order(), order );
						SetShaderValue( commandList, inscatterSCS.ThreadGroupZ(), i );

						commandList.BindPipelineState( inscatterSPSO.Get() );
						commandList.BindShaderResources( shaderBindings );
						commandList.Dispatch( INSCATTERS_GROUP_X, INSCATTERS_GROUP_Y );
					}
				} );

			// Compute deltaE
			BEGIN_RG_RESOURCE_STRUCT( DeltaEPassResource )
				DECLARE_RG_TEXTURE_NONPIXEL_SRV( deltaSR )
				DECLARE_RG_TEXTURE_NONPIXEL_SRV( deltaSM )
				DECLARE_RG_TEXTURE_UAV( deltaE )
			END_RG_RESOURCE_STRUCT();

			DeltaEPassResource deltaEPassResource = {
				.m_deltaSR = rgDeltaSR,
				.m_deltaSM = rgDeltaSM,
				.m_deltaE = rgDeltaE
			};

			renderGraph.AddPass(
				deltaEPassResource,
				[deltaEPassResource, order]( ComputeCommandList& commandList )
				{
					IrradianceNCS irradianceNCS;
					RefHandle<agl::ComputePipelineState> irradianceNPSO = PrepareComputePipelineState( irradianceNCS );
					commandList.BindPipelineState( irradianceNPSO.Get() );

					SamplerState pointSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

					agl::ShaderBindings shaderBindings = CreateShaderBindings( irradianceNCS );
					BindResource( shaderBindings, irradianceNCS.DeltaSRLut(), deltaEPassResource.m_deltaSR->Get() );
					BindResource( shaderBindings, irradianceNCS.DeltaSRLutSampler(), pointSampler );
					BindResource( shaderBindings, irradianceNCS.DeltaSMLut(), deltaEPassResource.m_deltaSM->Get() );
					BindResource( shaderBindings, irradianceNCS.DeltaSMLutSampler(), pointSampler );
					BindResource( shaderBindings, irradianceNCS.Irradiance(), deltaEPassResource.m_deltaE->Get() );
					SetShaderValue( commandList, irradianceNCS.Order(), order );

					commandList.BindShaderResources( shaderBindings );
					commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );
				} );
			
			// Compute deltaS
			BEGIN_RG_RESOURCE_STRUCT( DeltaSPassResource )
				DECLARE_RG_TEXTURE_NONPIXEL_SRV( transmittanceLut )
				DECLARE_RG_TEXTURE_NONPIXEL_SRV( deltaJ )
				DECLARE_RG_TEXTURE_UAV( deltaSR )
			END_RG_RESOURCE_STRUCT();

			DeltaSPassResource deltaSPassResource = {
				.m_transmittanceLut = rgTransmittanceLut,
				.m_deltaJ = rgDeltaJ,
				.m_deltaSR = rgDeltaSR
			};

			renderGraph.AddPass(
				deltaSPassResource,
				[deltaSPassResource]( ComputeCommandList& commandList )
				{
					InscatterNCS inscatterNCS;
					RefHandle<agl::ComputePipelineState> inscatterNPSO = PrepareComputePipelineState( inscatterNCS );
					commandList.BindPipelineState( inscatterNPSO.Get() );

					SamplerState pointSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

					agl::ShaderBindings shaderBindings = CreateShaderBindings( inscatterNCS );
					BindResource( shaderBindings, inscatterNCS.TransmittanceLut(), deltaSPassResource.m_transmittanceLut->Get() );
					BindResource( shaderBindings, inscatterNCS.TransmittanceLutSampler(), pointSampler );
					BindResource( shaderBindings, inscatterNCS.DeltaJLut(), deltaSPassResource.m_deltaJ->Get() );
					BindResource( shaderBindings, inscatterNCS.DeltaJLutSampler(), pointSampler );
					BindResource( shaderBindings, inscatterNCS.DeltaSR(), deltaSPassResource.m_deltaSR->Get() );

					commandList.BindShaderResources( shaderBindings );
					commandList.Dispatch( INSCATTERN_GROUP_X, INSCATTERN_GROUP_Y, INSCATTERN_GROUP_Z );
				} );

			// Add deltaE into irradiance textrue E
			BEGIN_RG_RESOURCE_STRUCT( CopyIrradiancePassResource )
				DECLARE_RG_TEXTURE_NONPIXEL_SRV( deltaE )
				DECLARE_RG_BUFFER_UAV( irradiance )
			END_RG_RESOURCE_STRUCT();

			CopyIrradiancePassResource copyIrradiancePassResource = {
				.m_deltaE = rgDeltaE,
				.m_irradiance = rgIrradiance
			};

			renderGraph.AddPass(
				copyIrradiancePassResource,
				[copyIrradiancePassResource]( ComputeCommandList& commandList )
				{
					CopyIrradianceCS copyIrradianceCS;
					RefHandle<agl::ComputePipelineState> copyIrradiancePSO = PrepareComputePipelineState( copyIrradianceCS );
					commandList.BindPipelineState( copyIrradiancePSO.Get() );

					SamplerState pointSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

					agl::ShaderBindings shaderBindings = CreateShaderBindings( copyIrradianceCS );

					BindResource( shaderBindings, copyIrradianceCS.DeltaELut(), copyIrradiancePassResource.m_deltaE->Get() );
					BindResource( shaderBindings, copyIrradianceCS.DeltaELutSampler(), pointSampler );
					BindResource( shaderBindings, copyIrradianceCS.Irradiance(), copyIrradiancePassResource.m_irradiance->Get() );

					commandList.BindShaderResources( shaderBindings );
					commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );
				} );

			// Add deltaS info inscatter texture S
			BEGIN_RG_RESOURCE_STRUCT( CopyInscatterPassResource )
				DECLARE_RG_TEXTURE_NONPIXEL_SRV( deltaSR )
				DECLARE_RG_BUFFER_UAV( inscatter )
			END_RG_RESOURCE_STRUCT();

			CopyInscatterPassResource copyInscatterPassResource = {
				.m_deltaSR = rgDeltaSR,
				.m_inscatter = rgInscatter
			};

			renderGraph.AddPass(
				copyInscatterPassResource,
				[copyInscatterPassResource]( ComputeCommandList& commandList )
				{
					CopyInscatterNCS copyInscatterNCS;
					RefHandle<agl::ComputePipelineState> copyInscatterNPSO = PrepareComputePipelineState( copyInscatterNCS );
					commandList.BindPipelineState( copyInscatterNPSO.Get() );

					SamplerState pointSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

					agl::ShaderBindings shaderBindings = CreateShaderBindings( copyInscatterNCS );

					BindResource( shaderBindings, copyInscatterNCS.DeltaSRLut(), copyInscatterPassResource.m_deltaSR->Get() );
					BindResource( shaderBindings, copyInscatterNCS.DeltaSRLutSampler(), pointSampler );
					BindResource( shaderBindings, copyInscatterNCS.Inscatter(), copyInscatterPassResource.m_inscatter->Get() );

					commandList.BindShaderResources( shaderBindings );
					commandList.Dispatch( INSCATTERN_GROUP_X, INSCATTERN_GROUP_Y, INSCATTERN_GROUP_Z );
				} );
		}

		// copy irradiance buffer to texture
		{
			agl::BufferDesc readback = {
				.m_stride = sizeof( Vector4 ),
				.m_count = IRRADIANCE_W * IRRADIANCE_H,
				.m_access = agl::ResourceAccess::Readback,
				.m_bindType = agl::ResourceBindType::None,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_format = agl::ResourceFormat::Unknown
			};

			RefHandle<agl::Buffer> irradianceReadback = agl::Buffer::Create( readback, "Atmosphere.IrradianceReadback" );

			auto rgIrradianceReadback = renderGraph.RegisterExternalResource( irradianceReadback.Get() );
			auto rgIrradianceLut = renderGraph.RegisterExternalResource( info.GetIrradianceLutTexture().Get() );

			BEGIN_RG_RESOURCE_STRUCT( CopyIrradianceReadbackPassResource )
				DECLARE_RG_BUFFER_COPY_DEST( irradianceReadback )
				DECLARE_RG_BUFFER_COPY_SOURCE( irradiance )
				DECLARE_RG_TEXTURE_COPY_DEST( irradianceLut )
			END_RG_RESOURCE_STRUCT();

			CopyIrradianceReadbackPassResource passResource = {
				.m_irradianceReadback = rgIrradianceReadback,
				.m_irradiance = rgIrradiance,
				.m_irradianceLut = rgIrradianceLut
			};

			renderGraph.AddPass(
				passResource,
				[passResource]( CopyCommandList& commandList )
				{
					commandList.CopyResource( passResource.m_irradianceReadback->Get(), passResource.m_irradiance->Get(), false);

					RenderGraph::Commit();

					GetInterface<agl::IAgl>()->WaitGPU();

					auto src = GraphicsInterface().Lock( passResource.m_irradianceReadback->Get(), agl::ResourceLockFlag::Read );
					auto srcData = static_cast<uint8*>( src.m_data );

					constexpr size_t RowSize = sizeof( Vector4 ) * IRRADIANCE_W;

					commandList.UpdateSubresource( passResource.m_irradianceLut->Get(), srcData, RowSize, false);
					GraphicsInterface().UnLock( passResource.m_irradianceReadback->Get() );
				});
		}

		// copy inscatter buffer to texture
		{
			agl::BufferDesc readback = {
				.m_stride = sizeof( Vector4 ),
				.m_count = RES_MU_S * RES_NU * RES_MU * RES_R,
				.m_access = agl::ResourceAccess::Readback,
				.m_bindType = agl::ResourceBindType::None,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_format = agl::ResourceFormat::Unknown
			};

			RefHandle<agl::Buffer> inscatterReadback = agl::Buffer::Create( readback, "Atmosphere.InscatterReadback" );

			auto rgInscatterReadback = renderGraph.RegisterExternalResource( inscatterReadback.Get() );
			auto rgInscatterLut = renderGraph.RegisterExternalResource( info.GetInscatterLutTexture().Get() );

			BEGIN_RG_RESOURCE_STRUCT( CopyInscatterReadbackPassResource )
				DECLARE_RG_BUFFER_COPY_DEST( inscatterReadback )
				DECLARE_RG_BUFFER_COPY_SOURCE( inscatter )
				DECLARE_RG_TEXTURE_COPY_DEST( inscatterLut )
			END_RG_RESOURCE_STRUCT();

			CopyInscatterReadbackPassResource passResource = {
				.m_inscatterReadback = rgInscatterReadback,
				.m_inscatter = rgInscatter,
				.m_inscatterLut = rgInscatterLut
			};

			renderGraph.AddPass(
				passResource,
				[passResource]( CopyCommandList& commandList )
				{
					commandList.CopyResource( passResource.m_inscatterReadback->Get(), passResource.m_inscatter->Get(), false);

					RenderGraph::Commit();

					GetInterface<agl::IAgl>()->WaitGPU();

					auto src = GraphicsInterface().Lock( passResource.m_inscatterReadback->Get(), agl::ResourceLockFlag::Read );
					auto srcData = static_cast<uint8*>( src.m_data );

					constexpr size_t RowSize = sizeof( Vector4 ) * RES_MU_S * RES_NU;

					commandList.UpdateSubresource( passResource.m_inscatterLut->Get(), srcData, RowSize, false);
					GraphicsInterface().UnLock( passResource.m_inscatterReadback->Get() );
				} );
		}

		info.RebuildLookUpTables() = false;
	}
}
