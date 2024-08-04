#include "SkyAtmosphereRendering.h"

#include "AbstractGraphicsInterface.h"
#include "AtmosphereConstant.h"
#include "CommandList.h"
#include "CommonRenderResource.h"
#include "ComputePipelineState.h"
#include "GlobalShaders.h"
#include "GpuProfiler.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"
#include "ShaderParameterUtils.h"
#include "StaticState.h"
#include "VertexCollection.h"

namespace rendercore
{
	class TransmittanceCS final : public GlobalShaderCommon<ComputeShader, TransmittanceCS>
	{
		DEFINE_SHADER_PARAM( Transmittance );
	};

	class IrradianceOneCS final : public GlobalShaderCommon<ComputeShader, IrradianceOneCS>
	{
		DEFINE_SHADER_PARAM( TransmittanceLut );
		DEFINE_SHADER_PARAM( TransmittanceLutSampler );
		DEFINE_SHADER_PARAM( DeltaE );
	};

	class InscatterOneCS final : public GlobalShaderCommon<ComputeShader, InscatterOneCS>
	{
		DEFINE_SHADER_PARAM( TransmittanceLut );
		DEFINE_SHADER_PARAM( TransmittanceLutSampler );
		DEFINE_SHADER_PARAM( DeltaSR );
		DEFINE_SHADER_PARAM( DeltaSM );
	};

	class CopyInscatterOneCS final : public GlobalShaderCommon<ComputeShader, CopyInscatterOneCS>
	{
		DEFINE_SHADER_PARAM( DeltaSRLut );
		DEFINE_SHADER_PARAM( DeltaSRLutSampler );
		DEFINE_SHADER_PARAM( DeltaSMLut );
		DEFINE_SHADER_PARAM( DeltaSMLutSampler );
		DEFINE_SHADER_PARAM( Inscatter );
	};

	class InscatterSCS final : public GlobalShaderCommon<ComputeShader, InscatterSCS>
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

	class IrradianceNCS final : public GlobalShaderCommon<ComputeShader, IrradianceNCS>
	{
		DEFINE_SHADER_PARAM( DeltaSRLut );
		DEFINE_SHADER_PARAM( DeltaSRLutSampler );
		DEFINE_SHADER_PARAM( DeltaSMLut );
		DEFINE_SHADER_PARAM( DeltaSMLutSampler );
		DEFINE_SHADER_PARAM( Order );
		DEFINE_SHADER_PARAM( Irradiance );
	};

	class InscatterNCS final : public GlobalShaderCommon<ComputeShader, InscatterNCS>
	{
		DEFINE_SHADER_PARAM( TransmittanceLut );
		DEFINE_SHADER_PARAM( TransmittanceLutSampler );
		DEFINE_SHADER_PARAM( DeltaJLut );
		DEFINE_SHADER_PARAM( DeltaJLutSampler );
		DEFINE_SHADER_PARAM( DeltaSR );
	};

	class CopyIrradianceCS final : public GlobalShaderCommon<ComputeShader, CopyIrradianceCS>
	{
		DEFINE_SHADER_PARAM( DeltaELut );
		DEFINE_SHADER_PARAM( DeltaELutSampler );
		DEFINE_SHADER_PARAM( Irradiance );
	};

	class CopyInscatterNCS final : public GlobalShaderCommon<ComputeShader, CopyInscatterNCS>
	{
		DEFINE_SHADER_PARAM( DeltaSRLut );
		DEFINE_SHADER_PARAM( DeltaSRLutSampler );
		DEFINE_SHADER_PARAM( Inscatter );
	};

	class DrawAtmospherePS final : public GlobalShaderCommon<PixelShader, DrawAtmospherePS>
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
				FullScreenQuadVS(),
				nullptr,
				DrawAtmospherePS()
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
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::None,
		};

		info.GetTransmittanceLutTexture() = agl::Texture::Create( transmittanceLut, "Atmosphere.TransmittanceLUT" );
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
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None,
		};

		info.GetIrradianceLutTexture() = agl::Texture::Create( irradianceLut, "Atmosphere.IrradianceLUT" );
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
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::Texture3D,
		};

		info.GetInscatterLutTexture() = agl::Texture::Create( inscatterLut, "Atmosphere.InscatterLUT" );
		EnqueueRenderTask(
			[texture = info.GetInscatterLutTexture()]()
			{
				texture->Init();
			} );

		info.GetShaderArguments() = SkyAtmosphereRenderParameters::CreateShaderArguments();
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

		auto commandList = GetCommandList();
		GPU_PROFILE( commandList, AtomosphereLookUp );

		SamplerState pointSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

		// 1. Transmittance Table
		TransmittanceCS transmittanceCS;

		RefHandle<agl::ComputePipelineState> transmittancePSO = PrepareComputePipelineState( transmittanceCS );
		commandList.BindPipelineState( transmittancePSO );

		commandList.AddTransition( Transition( *info.GetTransmittanceLutTexture().Get(), agl::ResourceState::UnorderedAccess) );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( transmittanceCS );
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
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::None
		};

		RefHandle<agl::Texture> deltaETexture = agl::Texture::Create( deltaE, "Atmosphere.DeltaE" );
		deltaETexture->Init();

		IrradianceOneCS irradianceOneCS;
		RefHandle<agl::ComputePipelineState> irradianceOnePSO = PrepareComputePipelineState( irradianceOneCS );
		commandList.BindPipelineState( irradianceOnePSO );

		commandList.AddTransition( Transition( *info.GetTransmittanceLutTexture().Get(), agl::ResourceState::NonPixelShaderResource ) );
		commandList.AddTransition( Transition( *deltaETexture.Get(), agl::ResourceState::UnorderedAccess ) );

		shaderBindings = CreateShaderBindings( irradianceOneCS );

		BindResource( shaderBindings, irradianceOneCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );
		BindResource( shaderBindings, irradianceOneCS.TransmittanceLutSampler(), pointSampler );
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
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::Texture3D
		};

		RefHandle<agl::Texture> deltaSRTexture = agl::Texture::Create( deltaS, "Atmosphere.DeltaSR" );
		deltaSRTexture->Init();

		RefHandle<agl::Texture> deltaSMTexture = agl::Texture::Create( deltaS, "Atmosphere.DeltaSM" );
		deltaSMTexture->Init();

		InscatterOneCS inscatterOneCS;
		RefHandle<agl::ComputePipelineState> inscatterOnePSO = PrepareComputePipelineState( inscatterOneCS );
		commandList.BindPipelineState( inscatterOnePSO );

		commandList.AddTransition( Transition( *info.GetTransmittanceLutTexture().Get(), agl::ResourceState::NonPixelShaderResource ) );
		commandList.AddTransition( Transition( *deltaSRTexture.Get(), agl::ResourceState::UnorderedAccess ) );
		commandList.AddTransition( Transition( *deltaSMTexture.Get(), agl::ResourceState::UnorderedAccess ) );

		shaderBindings = CreateShaderBindings( inscatterOneCS );

		BindResource( shaderBindings, inscatterOneCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );
		BindResource( shaderBindings, inscatterOneCS.TransmittanceLutSampler(), pointSampler );
		BindResource( shaderBindings, inscatterOneCS.DeltaSR(), deltaSRTexture );
		BindResource( shaderBindings, inscatterOneCS.DeltaSM(), deltaSMTexture );

		commandList.BindShaderResources( shaderBindings );
		commandList.Dispatch( INSCATTER1_GROUP_X, INSCATTER1_GROUP_Y, INSCATTER1_GROUP_Z );

		// 4. Copy deltaS into inscatter texture S
		agl::BufferTrait inscatter = {
			.m_stride = sizeof( Vector4 ),
			.m_count = RES_MU_S * RES_NU * RES_MU * RES_R,
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::BufferStructured,
			.m_format = agl::ResourceFormat::Unknown
		};

		RefHandle<agl::Buffer> inscatterBuffer = agl::Buffer::Create( inscatter, "Atmosphere.Inscatter" );
		inscatterBuffer->Init();

		CopyInscatterOneCS copyInscatterOneCS;
		RefHandle<agl::ComputePipelineState> copyInscatterOnePSO = PrepareComputePipelineState( copyInscatterOneCS );
		commandList.BindPipelineState( copyInscatterOnePSO );

		commandList.AddTransition( Transition( *deltaSRTexture.Get(), agl::ResourceState::NonPixelShaderResource ) );
		commandList.AddTransition( Transition( *deltaSMTexture.Get(), agl::ResourceState::NonPixelShaderResource ) );
		commandList.AddTransition( Transition( *inscatterBuffer.Get(), agl::ResourceState::UnorderedAccess ) );

		shaderBindings = CreateShaderBindings( copyInscatterOneCS );

		BindResource( shaderBindings, copyInscatterOneCS.DeltaSRLut(), deltaSRTexture );
		BindResource( shaderBindings, copyInscatterOneCS.DeltaSRLutSampler(), pointSampler );
		BindResource( shaderBindings, copyInscatterOneCS.DeltaSMLut(), deltaSMTexture );
		BindResource( shaderBindings, copyInscatterOneCS.DeltaSMLutSampler(), pointSampler );
		BindResource( shaderBindings, copyInscatterOneCS.Inscatter(), inscatterBuffer );

		commandList.BindShaderResources( shaderBindings );
		commandList.Dispatch( INSCATTER1_GROUP_X, INSCATTER1_GROUP_Y, INSCATTER1_GROUP_Z );

		RefHandle<agl::Texture> deltaJTex = agl::Texture::Create( deltaS, "Atmosphere.DeltaJ" );
		deltaJTex->Init();

		// Irradiance
		agl::BufferTrait irradiance = {
			.m_stride = sizeof( Vector4 ),
			.m_count = IRRADIANCE_W * IRRADIANCE_H,
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::BufferStructured,
			.m_format = agl::ResourceFormat::Unknown
		};

		Vector4 irradianceData[IRRADIANCE_W * IRRADIANCE_H] = {};
		RefHandle<agl::Buffer> irradianceBuffer = agl::Buffer::Create( irradiance, "Atmosphere.Irradiance", irradianceData );
		irradianceBuffer->Init();

		// loop for each scattering order
		for ( uint32 order = 2; order <= 4; ++order )
		{
			// Compute deltaJ
			InscatterSCS inscatterSCS;
			RefHandle<agl::ComputePipelineState> inscatterSPSO = PrepareComputePipelineState( inscatterSCS );

			shaderBindings = CreateShaderBindings( inscatterSCS );

			commandList.AddTransition( Transition( *info.GetTransmittanceLutTexture().Get(), agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *deltaETexture.Get(), agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *deltaSRTexture.Get(), agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *deltaSMTexture.Get(), agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *deltaJTex.Get(), agl::ResourceState::UnorderedAccess ) );

			BindResource( shaderBindings, inscatterSCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );
			BindResource( shaderBindings, inscatterSCS.TransmittanceLutSampler(), pointSampler );
			BindResource( shaderBindings, inscatterSCS.DeltaELut(), deltaETexture );
			BindResource( shaderBindings, inscatterSCS.DeltaELutSampler(), pointSampler );
			BindResource( shaderBindings, inscatterSCS.DeltaSRLut(), deltaSRTexture );
			BindResource( shaderBindings, inscatterSCS.DeltaSRLutSampler(), pointSampler );
			BindResource( shaderBindings, inscatterSCS.DeltaSMLut(), deltaSMTexture );
			BindResource( shaderBindings, inscatterSCS.DeltaSMLutSampler(), pointSampler );
			BindResource( shaderBindings, inscatterSCS.DeltaJ(), deltaJTex );

			for ( uint32 i = 0; i < INSCATTERS_GROUP_Z; ++i )
			{
				SetShaderValue( commandList, inscatterSCS.Order(), order );
				SetShaderValue( commandList, inscatterSCS.ThreadGroupZ(), i );

				commandList.BindPipelineState( inscatterSPSO );
				commandList.BindShaderResources( shaderBindings );
				commandList.Dispatch( INSCATTERS_GROUP_X, INSCATTERS_GROUP_Y );
			}

			// Compute deltaE
			IrradianceNCS irradianceNCS;
			RefHandle<agl::ComputePipelineState> irradianceNPSO = PrepareComputePipelineState( irradianceNCS );
			commandList.BindPipelineState( irradianceNPSO );

			shaderBindings = CreateShaderBindings( irradianceNCS );

			commandList.AddTransition( Transition( *deltaSRTexture.Get(), agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *deltaSMTexture.Get(), agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *deltaETexture.Get(), agl::ResourceState::UnorderedAccess ) );

			BindResource( shaderBindings, irradianceNCS.DeltaSRLut(), deltaSRTexture );
			BindResource( shaderBindings, irradianceNCS.DeltaSRLutSampler(), pointSampler );
			BindResource( shaderBindings, irradianceNCS.DeltaSMLut(), deltaSMTexture );
			BindResource( shaderBindings, irradianceNCS.DeltaSMLutSampler(), pointSampler );
			BindResource( shaderBindings, irradianceNCS.Irradiance(), deltaETexture );
			SetShaderValue( commandList, irradianceNCS.Order(), order );

			commandList.BindShaderResources( shaderBindings );
			commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

			// Compute deltaS
			InscatterNCS inscatterNCS;
			RefHandle<agl::ComputePipelineState> inscatterNPSO = PrepareComputePipelineState( inscatterNCS );
			commandList.BindPipelineState( inscatterNPSO );

			shaderBindings = CreateShaderBindings( inscatterNCS );

			commandList.AddTransition( Transition( *info.GetTransmittanceLutTexture().Get(), agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *deltaJTex.Get(), agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *deltaSRTexture.Get(), agl::ResourceState::UnorderedAccess ) );

			BindResource( shaderBindings, inscatterNCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );
			BindResource( shaderBindings, inscatterNCS.TransmittanceLutSampler(), pointSampler );
			BindResource( shaderBindings, inscatterNCS.DeltaJLut(), deltaJTex );
			BindResource( shaderBindings, inscatterNCS.DeltaJLutSampler(), pointSampler );
			BindResource( shaderBindings, inscatterNCS.DeltaSR(), deltaSRTexture );

			commandList.BindShaderResources( shaderBindings );
			commandList.Dispatch( INSCATTERN_GROUP_X, INSCATTERN_GROUP_Y, INSCATTERN_GROUP_Z );

			// Add deltaE into irradiance textrue E
			CopyIrradianceCS copyIrradianceCS;
			RefHandle<agl::ComputePipelineState> copyIrradiancePSO = PrepareComputePipelineState( copyIrradianceCS );
			commandList.BindPipelineState( copyIrradiancePSO );

			shaderBindings = CreateShaderBindings( copyIrradianceCS );

			commandList.AddTransition( Transition( *deltaETexture.Get(), agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *irradianceBuffer.Get(), agl::ResourceState::UnorderedAccess ) );

			BindResource( shaderBindings, copyIrradianceCS.DeltaELut(), deltaETexture );
			BindResource( shaderBindings, copyIrradianceCS.DeltaELutSampler(), pointSampler );
			BindResource( shaderBindings, copyIrradianceCS.Irradiance(), irradianceBuffer );

			commandList.BindShaderResources( shaderBindings );
			commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

			// Add deltaS info inscatter texture S
			CopyInscatterNCS copyInscatterNCS;
			RefHandle<agl::ComputePipelineState> copyInscatterNPSO = PrepareComputePipelineState( copyInscatterNCS );
			commandList.BindPipelineState( copyInscatterNPSO );

			shaderBindings = CreateShaderBindings( copyInscatterNCS );

			commandList.AddTransition( Transition( *deltaSRTexture.Get(), agl::ResourceState::NonPixelShaderResource ) );
			commandList.AddTransition( Transition( *inscatterBuffer.Get(), agl::ResourceState::UnorderedAccess ) );

			BindResource( shaderBindings, copyInscatterNCS.DeltaSRLut(), deltaSRTexture );
			BindResource( shaderBindings, copyInscatterNCS.DeltaSRLutSampler(), pointSampler );
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
				.m_access = agl::ResourceAccessFlag::Download,
				.m_bindType = agl::ResourceBindType::None,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_format = agl::ResourceFormat::Unknown
			};

			RefHandle<agl::Buffer> irradianceReadBack = agl::Buffer::Create( readBack, "Atmosphere.IrradianceReadBack" );
			irradianceReadBack->Init();

			commandList.AddTransition( Transition( *irradianceBuffer.Get(), agl::ResourceState::CopySource ) );
			commandList.CopyResource( irradianceReadBack, irradianceBuffer );

			auto src = GraphicsInterface().Lock( irradianceReadBack, agl::ResourceLockFlag::Read );
			auto srcData = static_cast<uint8*>( src.m_data );

			constexpr size_t rowSize = sizeof( Vector4 ) * IRRADIANCE_W;

			commandList.UpdateSubresource( info.GetIrradianceLutTexture(), srcData, rowSize );
			GraphicsInterface().UnLock( irradianceReadBack );
		}

		// copy inscatter buffer to texture
		{
			agl::BufferTrait readBack = {
				.m_stride = sizeof( Vector4 ),
				.m_count = RES_MU_S * RES_NU * RES_MU * RES_R,
				.m_access = agl::ResourceAccessFlag::Download,
				.m_bindType = agl::ResourceBindType::None,
				.m_miscFlag = agl::ResourceMisc::None,
				.m_format = agl::ResourceFormat::Unknown
			};

			RefHandle<agl::Buffer> inscatterReadBack = agl::Buffer::Create( readBack, "Atmosphere.InscatterReadBack" );
			inscatterReadBack->Init();

			commandList.AddTransition( Transition( *inscatterBuffer.Get(), agl::ResourceState::CopySource ) );
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
