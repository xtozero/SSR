#include "SkyAtmosphereRendering.h"

#include "AbstractGraphicsInterface.h"
#include "AtmosphereConstant.h"
#include "CommandList.h"
#include "CommonRenderResource.h"
#include "ComputePipelineState.h"
#include "GlobalShaders.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"
#include "ShaderParameterUtils.h"
#include "VertexCollection.h"

namespace rendercore
{
	class TransmittanceCS : public GlobalShaderCommon<ComputeShader, TransmittanceCS>
	{
		DEFINE_SHADER_PARAM( Transmittance );
	};

	class IrradianceOneCS : public GlobalShaderCommon<ComputeShader, IrradianceOneCS>
	{
		DEFINE_SHADER_PARAM( TransmittanceLut );
		DEFINE_SHADER_PARAM( TransmittanceLutSampler );
		DEFINE_SHADER_PARAM( DeltaE );
	};

	class InscatterOneCS : public GlobalShaderCommon<ComputeShader, InscatterOneCS>
	{
		DEFINE_SHADER_PARAM( TransmittanceLut );
		DEFINE_SHADER_PARAM( TransmittanceLutSampler );
		DEFINE_SHADER_PARAM( DeltaSR );
		DEFINE_SHADER_PARAM( DeltaSM );
	};

	class CopyInscatterOneCS : public GlobalShaderCommon<ComputeShader, CopyInscatterOneCS>
	{
		DEFINE_SHADER_PARAM( DeltaSRLut );
		DEFINE_SHADER_PARAM( DeltaSRLutSampler );
		DEFINE_SHADER_PARAM( DeltaSMLut );
		DEFINE_SHADER_PARAM( DeltaSMLutSampler );
		DEFINE_SHADER_PARAM( Inscatter );
	};

	class InscatterSCS : public GlobalShaderCommon<ComputeShader, InscatterSCS>
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

	class IrradianceNCS : public GlobalShaderCommon<ComputeShader, IrradianceNCS>
	{
		DEFINE_SHADER_PARAM( DeltaSRLut );
		DEFINE_SHADER_PARAM( DeltaSRLutSampler );
		DEFINE_SHADER_PARAM( DeltaSMLut );
		DEFINE_SHADER_PARAM( DeltaSMLutSampler );
		DEFINE_SHADER_PARAM( Order );
		DEFINE_SHADER_PARAM( Irradiance );
	};

	class InscatterNCS : public GlobalShaderCommon<ComputeShader, InscatterNCS>
	{
		DEFINE_SHADER_PARAM( TransmittanceLut );
		DEFINE_SHADER_PARAM( TransmittanceLutSampler );
		DEFINE_SHADER_PARAM( DeltaJLut );
		DEFINE_SHADER_PARAM( DeltaJLutSampler );
		DEFINE_SHADER_PARAM( DeltaSR );
	};

	class CopyIrradianceCS : public GlobalShaderCommon<ComputeShader, CopyIrradianceCS>
	{
		DEFINE_SHADER_PARAM( DeltaELut );
		DEFINE_SHADER_PARAM( DeltaELutSampler );
		DEFINE_SHADER_PARAM( Irradiance );
	};

	class CopyInscatterNCS : public GlobalShaderCommon<ComputeShader, CopyInscatterNCS>
	{
		DEFINE_SHADER_PARAM( DeltaSRLut );
		DEFINE_SHADER_PARAM( DeltaSRLutSampler );
		DEFINE_SHADER_PARAM( Inscatter );
	};

	class DrawAtmospherePS : public GlobalShaderCommon<PixelShader, DrawAtmospherePS>
	{};

	REGISTER_GLOBAL_SHADER( TransmittanceCS, "./Assets/Shaders/Atmosphere/CS_Transmittance.asset" );
	REGISTER_GLOBAL_SHADER( IrradianceOneCS, "./Assets/Shaders/Atmosphere/CS_Irradiance1.asset" );
	REGISTER_GLOBAL_SHADER( InscatterOneCS, "./Assets/Shaders/Atmosphere/CS_Inscatter1.asset" );
	REGISTER_GLOBAL_SHADER( CopyInscatterOneCS, "./Assets/Shaders/Atmosphere/CS_CopyInscatter1.asset" );
	REGISTER_GLOBAL_SHADER( InscatterSCS, "./Assets/Shaders/Atmosphere/CS_InscatterS.asset" );
	REGISTER_GLOBAL_SHADER( IrradianceNCS, "./Assets/Shaders/Atmosphere/CS_IrradianceN.asset" );
	REGISTER_GLOBAL_SHADER( InscatterNCS, "./Assets/Shaders/Atmosphere/CS_InscatterN.asset" );
	REGISTER_GLOBAL_SHADER( CopyIrradianceCS, "./Assets/Shaders/Atmosphere/CS_CopyIrradiance.asset" );
	REGISTER_GLOBAL_SHADER( CopyInscatterNCS, "./Assets/Shaders/Atmosphere/CS_CopyInscatterN.asset" );
	REGISTER_GLOBAL_SHADER( DrawAtmospherePS, "./Assets/Shaders/Atmosphere/PS_DrawAtmosphere.asset" );

	std::optional<DrawSnapshot> SkyAtmosphereDrawPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		PassShader passShader{
				FullScreenQuadVS().GetShader(),
				nullptr,
				DrawAtmospherePS().GetShader()
		};

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

	void InitAtmosphereForScene( Scene& scene )
	{
		SkyAtmosphereRenderSceneInfo& info = *scene.SkyAtmosphereSceneInfo();

		agl::TextureTrait transmittanceLut = {
			.m_width = TRANSMITTANCE_W,
			.m_height = TRANSMITTANCE_H,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
			.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::None,
		};

		info.GetTransmittanceLutTexture() = agl::Texture::Create( transmittanceLut );
		EnqueueRenderTask(
			[texture = info.GetTransmittanceLutTexture()]()
			{
				texture->Init();
			} );

		agl::TextureTrait irradianceLut = {
			.m_width = IRRADIANCE_W,
			.m_height = IRRADIANCE_H,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
			.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None,
		};

		info.GetIrradianceLutTexture() = agl::Texture::Create( irradianceLut );
		EnqueueRenderTask(
			[texture = info.GetIrradianceLutTexture()]()
			{
				texture->Init();
			} );

		agl::TextureTrait inscatterLut = {
			.m_width = RES_MU_S * RES_NU,
			.m_height = RES_MU,
			.m_depth = RES_R,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
			.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::Texture3D,
		};

		info.GetInscatterLutTexture() = agl::Texture::Create( inscatterLut );
		EnqueueRenderTask(
			[texture = info.GetInscatterLutTexture()]()
			{
				texture->Init();
			} );
	}

	void RenderAtmosphereLookUpTables( Scene& scene )
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

		SamplerOption pointSamperOption;
		pointSamperOption.m_filter = agl::TextureFilter::Point;
		auto pointSampler = GraphicsInterface().FindOrCreate( pointSamperOption );

		// 1. Transmittance Table
		TransmittanceCS transmittanceCS;

		auto commandList = GetCommandList();
		agl::RefHandle<agl::ComputePipelineState> transmittancePSO = PrepareComputePipelineState( transmittanceCS.GetShader()->Resource() );
		commandList.BindPipelineState( transmittancePSO );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( transmittanceCS.GetShader() );
		BindResource( shaderBindings, transmittanceCS.Transmittance(), info.GetTransmittanceLutTexture() );

		commandList.BindShaderResources( shaderBindings );
		commandList.Dispatch( TRANSMITTANCE_GROUP_X, TRANSMITTANCE_GROUP_Y );

		// 2. Ground irradiance due to direct sunlight
		agl::TextureTrait deltaE = {
			.m_width = IRRADIANCE_W,
			.m_height = IRRADIANCE_H,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
			.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::None
		};

		agl::RefHandle<agl::Texture> deltaETexture = agl::Texture::Create( deltaE );
		deltaETexture->Init();

		IrradianceOneCS irradianceOneCS;
		agl::RefHandle<agl::ComputePipelineState> irradianceOnePSO = PrepareComputePipelineState( irradianceOneCS.GetShader()->Resource() );
		commandList.BindPipelineState( irradianceOnePSO );

		shaderBindings = CreateShaderBindings( irradianceOneCS.GetShader() );

		BindResource( shaderBindings, irradianceOneCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );
		BindResource( shaderBindings, irradianceOneCS.TransmittanceLutSampler(), pointSampler.Resource() );
		BindResource( shaderBindings, irradianceOneCS.DeltaE(), deltaETexture );

		commandList.BindShaderResources( shaderBindings );
		commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

		// 3. Compute single scattering texture deltaS
		agl::TextureTrait deltaS = {
			.m_width = RES_MU_S * RES_NU,
			.m_height = RES_MU,
			.m_depth = RES_R,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
			.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::Texture3D
		};

		agl::RefHandle<agl::Texture> deltaSRTexture = agl::Texture::Create( deltaS );
		deltaSRTexture->Init();

		agl::RefHandle<agl::Texture> deltaSMTexture = agl::Texture::Create( deltaS );
		deltaSMTexture->Init();

		InscatterOneCS inscatterOneCS;
		agl::RefHandle<agl::ComputePipelineState> inscatterOnePSO = PrepareComputePipelineState( inscatterOneCS.GetShader()->Resource() );
		commandList.BindPipelineState( inscatterOnePSO );

		shaderBindings = CreateShaderBindings( inscatterOneCS.GetShader() );

		BindResource( shaderBindings, inscatterOneCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );
		BindResource( shaderBindings, inscatterOneCS.TransmittanceLutSampler(), pointSampler.Resource() );
		BindResource( shaderBindings, inscatterOneCS.DeltaSR(), deltaSRTexture );
		BindResource( shaderBindings, inscatterOneCS.DeltaSM(), deltaSMTexture );

		commandList.BindShaderResources( shaderBindings );
		commandList.Dispatch( INSCATTER1_GROUP_X, INSCATTER1_GROUP_Y, INSCATTER1_GROUP_Z );

		// 4. Copy deltaS into inscatter texture S
		agl::BufferTrait inscatter = {
			.m_stride = sizeof( Vector4 ),
			.m_count = RES_MU_S * RES_NU * RES_MU * RES_R,
			.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			.m_bindType = agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::BufferStructured,
			.m_format = agl::ResourceFormat::Unknown
		};

		agl::RefHandle<agl::Buffer> inscatterBuffer = agl::Buffer::Create( inscatter );
		inscatterBuffer->Init();

		CopyInscatterOneCS copyInscatterOneCS;
		agl::RefHandle<agl::ComputePipelineState> copyInscatterOnePSO = PrepareComputePipelineState( copyInscatterOneCS.GetShader()->Resource() );
		commandList.BindPipelineState( copyInscatterOnePSO );

		shaderBindings = CreateShaderBindings( copyInscatterOneCS.GetShader() );

		BindResource( shaderBindings, copyInscatterOneCS.DeltaSRLut(), deltaSRTexture );
		BindResource( shaderBindings, copyInscatterOneCS.DeltaSRLutSampler(), pointSampler.Resource() );
		BindResource( shaderBindings, copyInscatterOneCS.DeltaSMLut(), deltaSMTexture );
		BindResource( shaderBindings, copyInscatterOneCS.DeltaSMLutSampler(), pointSampler.Resource() );
		BindResource( shaderBindings, copyInscatterOneCS.Inscatter(), inscatterBuffer );

		commandList.BindShaderResources( shaderBindings );
		commandList.Dispatch( INSCATTER1_GROUP_X, INSCATTER1_GROUP_Y, INSCATTER1_GROUP_Z );

		agl::RefHandle<agl::Texture> deltaJTex = agl::Texture::Create( deltaS );
		deltaJTex->Init();

		// Irradiance
		agl::BufferTrait irradiance = {
			.m_stride = sizeof( Vector4 ),
			.m_count = IRRADIANCE_W * IRRADIANCE_H,
			.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			.m_bindType = agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::BufferStructured,
			.m_format = agl::ResourceFormat::Unknown
		};

		Vector4 irradianceData[IRRADIANCE_W * IRRADIANCE_H] = {};
		agl::RefHandle<agl::Buffer> irradianceBuffer = agl::Buffer::Create( irradiance, irradianceData );
		irradianceBuffer->Init();

		// loop for each scattering order
		for ( uint32 order = 2; order <= 4; ++order )
		{
			// Compute deltaJ
			InscatterSCS inscatterSCS;
			agl::RefHandle<agl::ComputePipelineState> inscatterSPSO = PrepareComputePipelineState( inscatterSCS.GetShader()->Resource() );
			commandList.BindPipelineState( inscatterSPSO );

			shaderBindings = CreateShaderBindings( inscatterSCS.GetShader() );

			BindResource( shaderBindings, inscatterSCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );
			BindResource( shaderBindings, inscatterSCS.TransmittanceLutSampler(), pointSampler.Resource() );
			BindResource( shaderBindings, inscatterSCS.DeltaELut(), deltaETexture );
			BindResource( shaderBindings, inscatterSCS.DeltaELutSampler(), pointSampler.Resource() );
			BindResource( shaderBindings, inscatterSCS.DeltaSRLut(), deltaSRTexture );
			BindResource( shaderBindings, inscatterSCS.DeltaSRLutSampler(), pointSampler.Resource() );
			BindResource( shaderBindings, inscatterSCS.DeltaSMLut(), deltaSMTexture );
			BindResource( shaderBindings, inscatterSCS.DeltaSMLutSampler(), pointSampler.Resource() );
			BindResource( shaderBindings, inscatterSCS.DeltaJ(), deltaJTex );

			for ( uint32 i = 0; i < INSCATTERS_GROUP_Z; ++i )
			{
				SetShaderValue( commandList, inscatterSCS.Order(), order );
				SetShaderValue( commandList, inscatterSCS.ThreadGroupZ(), i );

				commandList.BindShaderResources( shaderBindings );
				commandList.Dispatch( INSCATTERS_GROUP_X, INSCATTERS_GROUP_Y );
			}

			// Compute deltaE
			IrradianceNCS irradianceNCS;
			agl::RefHandle<agl::ComputePipelineState> irradianceNPSO = PrepareComputePipelineState( irradianceNCS.GetShader()->Resource() );
			commandList.BindPipelineState( irradianceNPSO );

			shaderBindings = CreateShaderBindings( irradianceNCS.GetShader() );

			BindResource( shaderBindings, irradianceNCS.DeltaSRLut(), deltaSRTexture );
			BindResource( shaderBindings, irradianceNCS.DeltaSRLutSampler(), pointSampler.Resource() );
			BindResource( shaderBindings, irradianceNCS.DeltaSMLut(), deltaSMTexture );
			BindResource( shaderBindings, irradianceNCS.DeltaSMLutSampler(), pointSampler.Resource() );
			BindResource( shaderBindings, irradianceNCS.Irradiance(), deltaETexture );
			SetShaderValue( commandList, irradianceNCS.Order(), order );

			commandList.BindShaderResources( shaderBindings );
			commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

			// Compute deltaS
			InscatterNCS inscatterNCS;
			agl::RefHandle<agl::ComputePipelineState> inscatterNPSO = PrepareComputePipelineState( inscatterNCS.GetShader()->Resource() );
			commandList.BindPipelineState( inscatterNPSO );

			shaderBindings = CreateShaderBindings( inscatterNCS.GetShader() );

			BindResource( shaderBindings, inscatterNCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );
			BindResource( shaderBindings, inscatterNCS.TransmittanceLutSampler(), pointSampler.Resource() );
			BindResource( shaderBindings, inscatterNCS.DeltaJLut(), deltaJTex );
			BindResource( shaderBindings, inscatterNCS.DeltaJLutSampler(), pointSampler.Resource() );
			BindResource( shaderBindings, inscatterNCS.DeltaSR(), deltaSRTexture );

			commandList.BindShaderResources( shaderBindings );
			commandList.Dispatch( INSCATTERN_GROUP_X, INSCATTERN_GROUP_Y, INSCATTERN_GROUP_Z );

			// Add deltaE into irradiance textrue E
			CopyIrradianceCS copyIrradianceCS;
			agl::RefHandle<agl::ComputePipelineState> copyIrradiancePSO = PrepareComputePipelineState( copyIrradianceCS.GetShader()->Resource() );
			commandList.BindPipelineState( copyIrradiancePSO );

			shaderBindings = CreateShaderBindings( copyIrradianceCS.GetShader() );

			BindResource( shaderBindings, copyIrradianceCS.DeltaELut(), deltaETexture );
			BindResource( shaderBindings, copyIrradianceCS.DeltaELutSampler(), pointSampler.Resource() );
			BindResource( shaderBindings, copyIrradianceCS.Irradiance(), irradianceBuffer );

			commandList.BindShaderResources( shaderBindings );
			commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

			// Add deltaS info inscatter texture S
			CopyInscatterNCS copyInscatterNCS;
			agl::RefHandle<agl::ComputePipelineState> copyInscatterNPSO = PrepareComputePipelineState( copyInscatterNCS.GetShader()->Resource() );
			commandList.BindPipelineState( copyInscatterNPSO );

			shaderBindings = CreateShaderBindings( copyInscatterNCS.GetShader() );

			BindResource( shaderBindings, copyInscatterNCS.DeltaSRLut(), deltaSRTexture );
			BindResource( shaderBindings, copyInscatterNCS.DeltaSRLutSampler(), pointSampler.Resource() );
			BindResource( shaderBindings, copyInscatterNCS.Inscatter(), inscatterBuffer );

			commandList.BindShaderResources( shaderBindings );
			commandList.Dispatch( INSCATTERN_GROUP_X, INSCATTERN_GROUP_Y, INSCATTERN_GROUP_Z );
		}

		commandList.Commit();
		GetInterface<agl::IAgl>()->WaitGPU();

		// copy irradiance buffer to texture
		{
			agl::BufferTrait readBack = {
				.m_stride = sizeof( Vector4 ),
				.m_count = IRRADIANCE_W * IRRADIANCE_H,
				.m_access = agl::ResourceAccessFlag::CpuRead,
				.m_bindType = agl::ResourceBindType::None,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_format = agl::ResourceFormat::Unknown
			};

			agl::RefHandle<agl::Buffer> irradianceReadBack = agl::Buffer::Create( readBack );
			irradianceReadBack->Init();

			commandList.CopyResource( irradianceReadBack, irradianceBuffer );

			auto src = GraphicsInterface().Lock( irradianceReadBack, agl::ResourceLockFlag::Read );
			auto srcData = static_cast<uint8*>( src.m_data );

			constexpr size_t rowSize = sizeof( Vector4 ) * IRRADIANCE_W;

			commandList.UpdateSubresource( info.GetIrradianceLutTexture(), srcData, rowSize);
			GraphicsInterface().UnLock( irradianceReadBack );
		}

		// copy inscatter buffer to texture
		{
			agl::BufferTrait readBack = {
				.m_stride = sizeof( Vector4 ),
				.m_count = RES_MU_S * RES_NU * RES_MU * RES_R,
				.m_access = agl::ResourceAccessFlag::CpuRead,
				.m_bindType = agl::ResourceBindType::None,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_format = agl::ResourceFormat::Unknown
			};

			agl::RefHandle<agl::Buffer> inscatterReadBack = agl::Buffer::Create( readBack );
			inscatterReadBack->Init();

			commandList.CopyResource( inscatterReadBack, inscatterBuffer );

			auto src = GraphicsInterface().Lock( inscatterReadBack, agl::ResourceLockFlag::Read );
			auto srcData = static_cast<uint8*>( src.m_data );

			constexpr size_t rowSize = sizeof( Vector4 ) * RES_MU_S * RES_NU;

			commandList.UpdateSubresource( info.GetInscatterLutTexture(), srcData, rowSize );
			GraphicsInterface().UnLock( inscatterReadBack );
		}

		info.RebuildLookUpTables() = false;
	}
}
