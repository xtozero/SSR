#include "SkyAtmosphereRendering.h"

#include "AbstractGraphicsInterface.h"
#include "AtmosphereConstant.h"
#include "CommandList.h"
#include "CommonRenderResource.h"
#include "GlobalShaders.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"
#include "ShaderParameterUtils.h"
#include "VertexCollection.h"

namespace rendercore
{
	class TransmittanceCS : public GlobalShaderCommon<ComputeShader, TransmittanceCS>
	{
	public:
		TransmittanceCS()
		{
			m_transmittance.Bind( GetShader()->ParameterMap(), "Transmittance" );
		}

		const agl::ShaderParameter& TransmittanceLut() const { return m_transmittance; }

	private:
		agl::ShaderParameter m_transmittance;
	};

	class IrradianceOneCS : public GlobalShaderCommon<ComputeShader, IrradianceOneCS>
	{
	public:
		IrradianceOneCS()
		{
			m_transmittanceLut.Bind( GetShader()->ParameterMap(), "TransmittanceLut" );
			m_transmittanceLutSampler.Bind( GetShader()->ParameterMap(), "TransmittanceLutSampler" );
			m_deltaE.Bind( GetShader()->ParameterMap(), "DeltaE" );
		}

		const agl::ShaderParameter& TransmittanceLut() const { return m_transmittanceLut; }
		const agl::ShaderParameter& TransmittanceLutSampler() const { return m_transmittanceLutSampler; }
		const agl::ShaderParameter& DeltaE() const { return m_deltaE; }

	private:
		agl::ShaderParameter m_transmittanceLut;
		agl::ShaderParameter m_transmittanceLutSampler;
		agl::ShaderParameter m_deltaE;
	};

	class InscatterOneCS : public GlobalShaderCommon<ComputeShader, InscatterOneCS>
	{
	public:
		InscatterOneCS()
		{
			m_transmittanceLut.Bind( GetShader()->ParameterMap(), "TransmittanceLut" );
			m_transmittanceLutSampler.Bind( GetShader()->ParameterMap(), "TransmittanceLutSampler" );
			m_deltaSR.Bind( GetShader()->ParameterMap(), "DeltaSR" );
			m_deltaSM.Bind( GetShader()->ParameterMap(), "DeltaSM" );
		}

		const agl::ShaderParameter& TransmittanceLut() const { return m_transmittanceLut; }
		const agl::ShaderParameter& TransmittanceLutSampler() const { return m_transmittanceLutSampler; }
		const agl::ShaderParameter& DeltaSR() const { return m_deltaSR; }
		const agl::ShaderParameter& DeltaSM() const { return m_deltaSM; }

	private:
		agl::ShaderParameter m_transmittanceLut;
		agl::ShaderParameter m_transmittanceLutSampler;
		agl::ShaderParameter m_deltaSR;
		agl::ShaderParameter m_deltaSM;
	};

	class CopyInscatterOneCS : public GlobalShaderCommon<ComputeShader, CopyInscatterOneCS>
	{
	public:
		CopyInscatterOneCS()
		{
			m_deltaSRLut.Bind( GetShader()->ParameterMap(), "DeltaSRLut" );
			m_deltaSRLutSampler.Bind( GetShader()->ParameterMap(), "DeltaSRLutSampler" );
			m_deltaSMLut.Bind( GetShader()->ParameterMap(), "DeltaSMLut" );
			m_deltaSMLutSampler.Bind( GetShader()->ParameterMap(), "DeltaSMLutSampler" );
			m_inscatter.Bind( GetShader()->ParameterMap(), "Inscatter" );
		}

		const agl::ShaderParameter& DeltaSRLut() const { return m_deltaSRLut; }
		const agl::ShaderParameter& DeltaSRLutSampler() const { return m_deltaSRLutSampler; }
		const agl::ShaderParameter& DeltaSMLut() const { return m_deltaSMLut; }
		const agl::ShaderParameter& DeltaSMLutSampler() const { return m_deltaSMLutSampler; }
		const agl::ShaderParameter& Inscatter() const { return m_inscatter; }

	private:
		agl::ShaderParameter m_deltaSRLut;
		agl::ShaderParameter m_deltaSRLutSampler;
		agl::ShaderParameter m_deltaSMLut;
		agl::ShaderParameter m_deltaSMLutSampler;
		agl::ShaderParameter m_inscatter;
	};

	class InscatterSCS : public GlobalShaderCommon<ComputeShader, InscatterSCS>
	{
	public:
		InscatterSCS()
		{
			m_deltaELut.Bind( GetShader()->ParameterMap(), "DeltaELut" );
			m_deltaELutSampler.Bind( GetShader()->ParameterMap(), "DeltaELutSampler" );
			m_deltaSRLut.Bind( GetShader()->ParameterMap(), "DeltaSRLut" );
			m_deltaSRLutSampler.Bind( GetShader()->ParameterMap(), "DeltaSRLutSampler" );
			m_deltaSMLut.Bind( GetShader()->ParameterMap(), "DeltaSMLut" );
			m_deltaSMLutSampler.Bind( GetShader()->ParameterMap(), "DeltaSMLutSampler" );
			m_order.Bind( GetShader()->ParameterMap(), "Order" );
			m_threadGroupZ.Bind( GetShader()->ParameterMap(), "ThreadGroupZ" );
			m_deltaJ.Bind( GetShader()->ParameterMap(), "DeltaJ" );
		}

		const agl::ShaderParameter& DeltaELut() const { return m_deltaELut; }
		const agl::ShaderParameter& DeltaELutSampler() const { return m_deltaELutSampler; }
		const agl::ShaderParameter& DeltaSRLut() const { return m_deltaSRLut; }
		const agl::ShaderParameter& DeltaSRLutSampler() const { return m_deltaSRLutSampler; }
		const agl::ShaderParameter& DeltaSMLut() const { return m_deltaSMLut; }
		const agl::ShaderParameter& DeltaSMLutSampler() const { return m_deltaSMLutSampler; }
		const agl::ShaderParameter& Order() const { return m_order; }
		const agl::ShaderParameter& ThreadGroupZ() const { return m_threadGroupZ; }
		const agl::ShaderParameter& DeltaJ() const { return m_deltaJ; }

	private:
		agl::ShaderParameter m_deltaELut;
		agl::ShaderParameter m_deltaELutSampler;
		agl::ShaderParameter m_deltaSRLut;
		agl::ShaderParameter m_deltaSRLutSampler;
		agl::ShaderParameter m_deltaSMLut;
		agl::ShaderParameter m_deltaSMLutSampler;
		agl::ShaderParameter m_order;
		agl::ShaderParameter m_threadGroupZ;
		agl::ShaderParameter m_deltaJ;
	};

	class IrradianceNCS : public GlobalShaderCommon<ComputeShader, IrradianceNCS>
	{
	public:
		IrradianceNCS()
		{
			m_deltaSRLut.Bind( GetShader()->ParameterMap(), "DeltaSRLut" );
			m_deltaSRLutSampler.Bind( GetShader()->ParameterMap(), "DeltaSRLutSampler" );
			m_deltaSMLut.Bind( GetShader()->ParameterMap(), "DeltaSMLut" );
			m_deltaSMLutSampler.Bind( GetShader()->ParameterMap(), "DeltaSMLutSampler" );
			m_order.Bind( GetShader()->ParameterMap(), "Order" );
			m_irradiance.Bind( GetShader()->ParameterMap(), "Irradiance" );
		}

		const agl::ShaderParameter& DeltaSRLut() const { return m_deltaSRLut; }
		const agl::ShaderParameter& DeltaSRLutSampler() const { return m_deltaSRLutSampler; }
		const agl::ShaderParameter& DeltaSMLut() const { return m_deltaSMLut; }
		const agl::ShaderParameter& DeltaSMLutSampler() const { return m_deltaSMLutSampler; }
		const agl::ShaderParameter& Order() const { return m_order; }
		const agl::ShaderParameter& Irradiance() const { return m_irradiance; }

	private:
		agl::ShaderParameter m_deltaSRLut;
		agl::ShaderParameter m_deltaSRLutSampler;
		agl::ShaderParameter m_deltaSMLut;
		agl::ShaderParameter m_deltaSMLutSampler;
		agl::ShaderParameter m_order;
		agl::ShaderParameter m_irradiance;
	};

	class InscatterNCS : public GlobalShaderCommon<ComputeShader, InscatterNCS>
	{
	public:
		InscatterNCS()
		{
			m_deltaJLut.Bind( GetShader()->ParameterMap(), "DeltaJLut" );
			m_deltaJLutSampler.Bind( GetShader()->ParameterMap(), "DeltaJLutSampler" );
			m_deltaSR.Bind( GetShader()->ParameterMap(), "DeltaSR" );
		}

		const agl::ShaderParameter& DeltaJLut() const { return m_deltaJLut; }
		const agl::ShaderParameter& DeltaJLutSampler() const { return m_deltaJLutSampler; }
		const agl::ShaderParameter& DeltaSR() const { return m_deltaSR; }

	private:
		agl::ShaderParameter m_deltaJLut;
		agl::ShaderParameter m_deltaJLutSampler;
		agl::ShaderParameter m_deltaSR;
	};

	class CopyIrradianceCS : public GlobalShaderCommon<ComputeShader, CopyIrradianceCS>
	{
	public:
		CopyIrradianceCS()
		{
			m_deltaELut.Bind( GetShader()->ParameterMap(), "DeltaELut" );
			m_deltaELutSampler.Bind( GetShader()->ParameterMap(), "DeltaELutSampler" );
			m_irradiance.Bind( GetShader()->ParameterMap(), "Irradiance" );
		}

		const agl::ShaderParameter& DeltaELut() const { return m_deltaELut; }
		const agl::ShaderParameter& DeltaELutSampler() const { return m_deltaELutSampler; }
		const agl::ShaderParameter& Irradiance() const { return m_irradiance; }

	private:
		agl::ShaderParameter m_deltaELut;
		agl::ShaderParameter m_deltaELutSampler;
		agl::ShaderParameter m_irradiance;
	};

	class CopyInscatterNCS : public GlobalShaderCommon<ComputeShader, CopyInscatterNCS>
	{
	public:
		CopyInscatterNCS()
		{
			m_deltaSRLut.Bind( GetShader()->ParameterMap(), "DeltaSRLut" );
			m_deltaSRLutSampler.Bind( GetShader()->ParameterMap(), "DeltaSRLutSampler" );
			m_inscatter.Bind( GetShader()->ParameterMap(), "Inscatter" );
		}

		const agl::ShaderParameter& DeltaSRLut() const { return m_deltaSRLut; }
		const agl::ShaderParameter& DeltaSRLutSampler() const { return m_deltaSRLutSampler; }
		const agl::ShaderParameter& Irradiance() const { return m_inscatter; }

	private:
		agl::ShaderParameter m_deltaSRLut;
		agl::ShaderParameter m_deltaSRLutSampler;
		agl::ShaderParameter m_inscatter;
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

		agl::TEXTURE_TRAIT transmittanceLut = {
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

		agl::TEXTURE_TRAIT irradianceLut = {
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

		agl::TEXTURE_TRAIT inscatterLut = {
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

		auto commandList = GetImmediateCommandList();
		commandList.BindShader( transmittanceCS.GetShader()->Resource() );
		BindShaderParameter( commandList, transmittanceCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );

		commandList.Dispatch( TRANSMITTANCE_GROUP_X, TRANSMITTANCE_GROUP_Y );

		// 2. Ground irradiance due to direct sunlight
		agl::TEXTURE_TRAIT deltaE = {
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
		commandList.BindShader( irradianceOneCS.GetShader()->Resource() );
		BindShaderParameter( commandList, irradianceOneCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );
		BindShaderParameter( commandList, irradianceOneCS.TransmittanceLutSampler(), pointSampler.Resource() );
		BindShaderParameter( commandList, irradianceOneCS.DeltaE(), deltaETexture );

		commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

		// 3. Compute single scattering texture deltaS
		agl::TEXTURE_TRAIT deltaS = {
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
		commandList.BindShader( inscatterOneCS.GetShader()->Resource() );
		BindShaderParameter( commandList, inscatterOneCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );
		BindShaderParameter( commandList, inscatterOneCS.TransmittanceLutSampler(), pointSampler.Resource() );
		BindShaderParameter( commandList, inscatterOneCS.DeltaSR(), deltaSRTexture );
		BindShaderParameter( commandList, inscatterOneCS.DeltaSM(), deltaSMTexture );

		commandList.Dispatch( INSCATTER1_GROUP_X, INSCATTER1_GROUP_Y, INSCATTER1_GROUP_Z );

		// 4. Copy deltaS into inscatter texture S
		agl::BUFFER_TRAIT inscatter = {
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
		commandList.BindShader( copyInscatterOneCS.GetShader()->Resource() );
		BindShaderParameter( commandList, copyInscatterOneCS.DeltaSRLut(), deltaSRTexture );
		BindShaderParameter( commandList, copyInscatterOneCS.DeltaSRLutSampler(), pointSampler.Resource() );
		BindShaderParameter( commandList, copyInscatterOneCS.DeltaSMLut(), deltaSMTexture );
		BindShaderParameter( commandList, copyInscatterOneCS.DeltaSMLutSampler(), pointSampler.Resource() );
		BindShaderParameter( commandList, copyInscatterOneCS.Inscatter(), inscatterBuffer );

		commandList.Dispatch( INSCATTER1_GROUP_X, INSCATTER1_GROUP_Y, INSCATTER1_GROUP_Z );

		agl::RefHandle<agl::Texture> deltaJTex = agl::Texture::Create( deltaS );
		deltaJTex->Init();

		// Irradiance
		agl::BUFFER_TRAIT irradiance = {
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
			commandList.BindShader( inscatterSCS.GetShader()->Resource() );

			BindShaderParameter( commandList, inscatterSCS.DeltaELut(), deltaETexture );
			BindShaderParameter( commandList, inscatterSCS.DeltaELutSampler(), pointSampler.Resource() );
			BindShaderParameter( commandList, inscatterSCS.DeltaSRLut(), deltaSRTexture );
			BindShaderParameter( commandList, inscatterSCS.DeltaSRLutSampler(), pointSampler.Resource() );
			BindShaderParameter( commandList, inscatterSCS.DeltaSMLut(), deltaSMTexture );
			BindShaderParameter( commandList, inscatterSCS.DeltaSMLutSampler(), pointSampler.Resource() );
			BindShaderParameter( commandList, inscatterSCS.DeltaJ(), deltaJTex );

			for ( uint32 i = 0; i < INSCATTERS_GROUP_Z; ++i )
			{
				SetShaderValue( inscatterSCS.Order(), order );
				SetShaderValue( inscatterSCS.ThreadGroupZ(), i );

				commandList.Dispatch( INSCATTERS_GROUP_X, INSCATTERS_GROUP_Y );
				commandList.WaitUntilFlush();
			}

			// Compute deltaE
			IrradianceNCS irradianceNCS;
			commandList.BindShader( irradianceNCS.GetShader()->Resource() );
			BindShaderParameter( commandList, irradianceNCS.DeltaSRLut(), deltaSRTexture );
			BindShaderParameter( commandList, irradianceNCS.DeltaSRLutSampler(), pointSampler.Resource() );
			BindShaderParameter( commandList, irradianceNCS.DeltaSMLut(), deltaSMTexture );
			BindShaderParameter( commandList, irradianceNCS.DeltaSMLutSampler(), pointSampler.Resource() );
			BindShaderParameter( commandList, irradianceNCS.Irradiance(), deltaETexture );
			SetShaderValue( irradianceNCS.Order(), order );

			commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

			// Compute deltaS
			InscatterNCS inscatterNCS;
			commandList.BindShader( inscatterNCS.GetShader()->Resource() );
			BindShaderParameter( commandList, inscatterNCS.DeltaJLut(), deltaJTex );
			BindShaderParameter( commandList, inscatterNCS.DeltaJLutSampler(), pointSampler.Resource() );
			BindShaderParameter( commandList, inscatterNCS.DeltaSR(), deltaSRTexture );

			commandList.Dispatch( INSCATTERN_GROUP_X, INSCATTERN_GROUP_Y, INSCATTERN_GROUP_Z );

			// Add deltaE into irradiance textrue E
			CopyIrradianceCS copyIrradianceCS;
			commandList.BindShader( copyIrradianceCS.GetShader()->Resource() );
			BindShaderParameter( commandList, copyIrradianceCS.DeltaELut(), deltaETexture );
			BindShaderParameter( commandList, copyIrradianceCS.DeltaELutSampler(), pointSampler.Resource() );
			BindShaderParameter( commandList, copyIrradianceCS.Irradiance(), irradianceBuffer );

			commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

			// Add deltaS info inscatter texture S
			CopyInscatterNCS copyInscatterNCS;
			commandList.BindShader( copyInscatterNCS.GetShader()->Resource() );
			BindShaderParameter( commandList, copyInscatterNCS.DeltaSRLut(), deltaSRTexture );
			BindShaderParameter( commandList, copyInscatterNCS.DeltaSRLutSampler(), pointSampler.Resource() );
			BindShaderParameter( commandList, copyInscatterNCS.Irradiance(), inscatterBuffer );

			commandList.Dispatch( INSCATTERN_GROUP_X, INSCATTERN_GROUP_Y, INSCATTERN_GROUP_Z );
		}

		// copy irradiance buffer to texture
		{
			agl::BUFFER_TRAIT readBack = {
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

			agl::TEXTURE_TRAIT intermediate = {
				.m_width = IRRADIANCE_W,
				.m_height = IRRADIANCE_H,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
				.m_access = agl::ResourceAccessFlag::CpuWrite,
				.m_bindType = agl::ResourceBindType::None,
				.m_miscFlag = agl::ResourceMisc::Intermediate
			};

			agl::RefHandle<agl::Texture> irradianceIntermedicate = agl::Texture::Create( intermediate );
			irradianceIntermedicate->Init();

			auto src = GraphicsInterface().Lock( irradianceReadBack, agl::ResourceLockFlag::Read );
			auto dest = GraphicsInterface().Lock( irradianceIntermedicate, agl::ResourceLockFlag::WriteDiscard );

			auto srcData = static_cast<uint8*>( src.m_data );
			auto destData = static_cast<uint8*>( dest.m_data );
			for ( uint32 y = 0; y < IRRADIANCE_H; ++y )
			{
				constexpr size_t size = sizeof( Vector4 ) * IRRADIANCE_W;
				std::memcpy( destData, srcData, size );
				srcData += size;
				destData += dest.m_rowPitch;
			}

			GraphicsInterface().UnLock( irradianceReadBack );
			GraphicsInterface().UnLock( irradianceIntermedicate );

			commandList.CopyResource( info.GetIrradianceLutTexture(), irradianceIntermedicate );
		}

		// copy inscatter buffer to texture
		{
			agl::BUFFER_TRAIT readBack = {
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

			agl::TEXTURE_TRAIT intermediate = {
				.m_width = RES_MU_S * RES_NU,
				.m_height = RES_MU,
				.m_depth = RES_R,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
				.m_access = agl::ResourceAccessFlag::CpuWrite,
				.m_bindType = agl::ResourceBindType::None,
				.m_miscFlag = agl::ResourceMisc::Texture3D | agl::ResourceMisc::Intermediate
			};

			agl::RefHandle<agl::Texture> inscatterIntermedicate = agl::Texture::Create( intermediate );
			inscatterIntermedicate->Init();

			auto src = GraphicsInterface().Lock( inscatterReadBack, agl::ResourceLockFlag::Read );
			auto dest = GraphicsInterface().Lock( inscatterIntermedicate, agl::ResourceLockFlag::WriteDiscard );

			auto srcData = static_cast<uint8*>( src.m_data );
			auto destData = static_cast<uint8*>( dest.m_data );
			for ( uint32 z = 0; z < RES_R; ++z )
			{
				uint8* row = destData;
				for ( uint32 y = 0; y < RES_MU; ++y )
				{
					constexpr size_t size = sizeof( Vector4 ) * RES_MU_S * RES_NU;
					std::memcpy( row, srcData, size );
					srcData += size;
					row += dest.m_rowPitch;
				}
				destData += dest.m_depthPitch;
			}

			GraphicsInterface().UnLock( inscatterReadBack );
			GraphicsInterface().UnLock( inscatterIntermedicate );

			commandList.CopyResource( info.GetInscatterLutTexture(), inscatterIntermedicate );
		}

		info.RebuildLookUpTables() = false;
	}
}
