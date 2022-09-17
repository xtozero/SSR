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
	class TransmittanceCS
	{
	public:
		TransmittanceCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<TransmittanceCS>()->CompileShader( {} ) );

			if ( m_shader )
			{
				m_transmittance.Bind( m_shader->ParameterMap(), "Transmittance" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const agl::ShaderParameter& TransmittanceLut() const { return m_transmittance; }

	private:
		ComputeShader* m_shader = nullptr;

		agl::ShaderParameter m_transmittance;
	};

	class IrradianceOneCS
	{
	public:
		IrradianceOneCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<IrradianceOneCS>()->CompileShader( {} ) );

			if ( m_shader )
			{
				m_transmittanceLut.Bind( m_shader->ParameterMap(), "TransmittanceLut" );
				m_transmittanceLutSampler.Bind( m_shader->ParameterMap(), "TransmittanceLutSampler" );
				m_deltaE.Bind( m_shader->ParameterMap(), "DeltaE" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const agl::ShaderParameter& TransmittanceLut() const { return m_transmittanceLut; }
		const agl::ShaderParameter& TransmittanceLutSampler() const { return m_transmittanceLutSampler; }
		const agl::ShaderParameter& DeltaE() const { return m_deltaE; }

	private:
		ComputeShader* m_shader = nullptr;

		agl::ShaderParameter m_transmittanceLut;
		agl::ShaderParameter m_transmittanceLutSampler;
		agl::ShaderParameter m_deltaE;
	};

	class InscatterOneCS
	{
	public:
		InscatterOneCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<InscatterOneCS>()->CompileShader( {} ) );

			if ( m_shader )
			{
				m_transmittanceLut.Bind( m_shader->ParameterMap(), "TransmittanceLut" );
				m_transmittanceLutSampler.Bind( m_shader->ParameterMap(), "TransmittanceLutSampler" );
				m_deltaSR.Bind( m_shader->ParameterMap(), "DeltaSR" );
				m_deltaSM.Bind( m_shader->ParameterMap(), "DeltaSM" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const agl::ShaderParameter& TransmittanceLut() const { return m_transmittanceLut; }
		const agl::ShaderParameter& TransmittanceLutSampler() const { return m_transmittanceLutSampler; }
		const agl::ShaderParameter& DeltaSR() const { return m_deltaSR; }
		const agl::ShaderParameter& DeltaSM() const { return m_deltaSM; }

	private:
		ComputeShader* m_shader = nullptr;

		agl::ShaderParameter m_transmittanceLut;
		agl::ShaderParameter m_transmittanceLutSampler;
		agl::ShaderParameter m_deltaSR;
		agl::ShaderParameter m_deltaSM;
	};

	class CopyInscatterOneCS
	{
	public:
		CopyInscatterOneCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<CopyInscatterOneCS>()->CompileShader( {} ) );

			if ( m_shader )
			{
				m_deltaSRLut.Bind( m_shader->ParameterMap(), "DeltaSRLut" );
				m_deltaSRLutSampler.Bind( m_shader->ParameterMap(), "DeltaSRLutSampler" );
				m_deltaSMLut.Bind( m_shader->ParameterMap(), "DeltaSMLut" );
				m_deltaSMLutSampler.Bind( m_shader->ParameterMap(), "DeltaSMLutSampler" );
				m_inscatter.Bind( m_shader->ParameterMap(), "Inscatter" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const agl::ShaderParameter& DeltaSRLut() const { return m_deltaSRLut; }
		const agl::ShaderParameter& DeltaSRLutSampler() const { return m_deltaSRLutSampler; }
		const agl::ShaderParameter& DeltaSMLut() const { return m_deltaSMLut; }
		const agl::ShaderParameter& DeltaSMLutSampler() const { return m_deltaSMLutSampler; }
		const agl::ShaderParameter& Inscatter() const { return m_inscatter; }

	private:
		ComputeShader* m_shader = nullptr;

		agl::ShaderParameter m_deltaSRLut;
		agl::ShaderParameter m_deltaSRLutSampler;
		agl::ShaderParameter m_deltaSMLut;
		agl::ShaderParameter m_deltaSMLutSampler;
		agl::ShaderParameter m_inscatter;
	};

	class InscatterSCS
	{
	public:
		InscatterSCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<InscatterSCS>()->CompileShader( {} ) );

			if ( m_shader )
			{
				m_deltaELut.Bind( m_shader->ParameterMap(), "DeltaELut" );
				m_deltaELutSampler.Bind( m_shader->ParameterMap(), "DeltaELutSampler" );
				m_deltaSRLut.Bind( m_shader->ParameterMap(), "DeltaSRLut" );
				m_deltaSRLutSampler.Bind( m_shader->ParameterMap(), "DeltaSRLutSampler" );
				m_deltaSMLut.Bind( m_shader->ParameterMap(), "DeltaSMLut" );
				m_deltaSMLutSampler.Bind( m_shader->ParameterMap(), "DeltaSMLutSampler" );
				m_order.Bind( m_shader->ParameterMap(), "Order" );
				m_threadGroupZ.Bind( m_shader->ParameterMap(), "ThreadGroupZ" );
				m_deltaJ.Bind( m_shader->ParameterMap(), "DeltaJ" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
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
		ComputeShader* m_shader = nullptr;

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

	class IrradianceNCS
	{
	public:
		IrradianceNCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<IrradianceNCS>()->CompileShader( {} ) );

			if ( m_shader )
			{
				m_deltaSRLut.Bind( m_shader->ParameterMap(), "DeltaSRLut" );
				m_deltaSRLutSampler.Bind( m_shader->ParameterMap(), "DeltaSRLutSampler" );
				m_deltaSMLut.Bind( m_shader->ParameterMap(), "DeltaSMLut" );
				m_deltaSMLutSampler.Bind( m_shader->ParameterMap(), "DeltaSMLutSampler" );
				m_order.Bind( m_shader->ParameterMap(), "Order" );
				m_irradiance.Bind( m_shader->ParameterMap(), "Irradiance" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const agl::ShaderParameter& DeltaSRLut() const { return m_deltaSRLut; }
		const agl::ShaderParameter& DeltaSRLutSampler() const { return m_deltaSRLutSampler; }
		const agl::ShaderParameter& DeltaSMLut() const { return m_deltaSMLut; }
		const agl::ShaderParameter& DeltaSMLutSampler() const { return m_deltaSMLutSampler; }
		const agl::ShaderParameter& Order() const { return m_order; }
		const agl::ShaderParameter& Irradiance() const { return m_irradiance; }

	private:
		ComputeShader* m_shader = nullptr;

		agl::ShaderParameter m_deltaSRLut;
		agl::ShaderParameter m_deltaSRLutSampler;
		agl::ShaderParameter m_deltaSMLut;
		agl::ShaderParameter m_deltaSMLutSampler;
		agl::ShaderParameter m_order;
		agl::ShaderParameter m_irradiance;
	};

	class InscatterNCS
	{
	public:
		InscatterNCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<InscatterNCS>()->CompileShader( {} ) );

			if ( m_shader )
			{
				m_deltaJLut.Bind( m_shader->ParameterMap(), "DeltaJLut" );
				m_deltaJLutSampler.Bind( m_shader->ParameterMap(), "DeltaJLutSampler" );
				m_deltaSR.Bind( m_shader->ParameterMap(), "DeltaSR" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const agl::ShaderParameter& DeltaJLut() const { return m_deltaJLut; }
		const agl::ShaderParameter& DeltaJLutSampler() const { return m_deltaJLutSampler; }
		const agl::ShaderParameter& DeltaSR() const { return m_deltaSR; }

	private:
		ComputeShader* m_shader = nullptr;

		agl::ShaderParameter m_deltaJLut;
		agl::ShaderParameter m_deltaJLutSampler;
		agl::ShaderParameter m_deltaSR;
	};

	class CopyIrradianceCS
	{
	public:
		CopyIrradianceCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<CopyIrradianceCS>()->CompileShader( {} ) );

			if ( m_shader )
			{
				m_deltaELut.Bind( m_shader->ParameterMap(), "DeltaELut" );
				m_deltaELutSampler.Bind( m_shader->ParameterMap(), "DeltaELutSampler" );
				m_irradiance.Bind( m_shader->ParameterMap(), "Irradiance" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const agl::ShaderParameter& DeltaELut() const { return m_deltaELut; }
		const agl::ShaderParameter& DeltaELutSampler() const { return m_deltaELutSampler; }
		const agl::ShaderParameter& Irradiance() const { return m_irradiance; }

	private:
		ComputeShader* m_shader = nullptr;

		agl::ShaderParameter m_deltaELut;
		agl::ShaderParameter m_deltaELutSampler;
		agl::ShaderParameter m_irradiance;
	};

	class CopyInscatterNCS
	{
	public:
		CopyInscatterNCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<CopyInscatterNCS>()->CompileShader( {} ) );

			if ( m_shader )
			{
				m_deltaSRLut.Bind( m_shader->ParameterMap(), "DeltaSRLut" );
				m_deltaSRLutSampler.Bind( m_shader->ParameterMap(), "DeltaSRLutSampler" );
				m_inscatter.Bind( m_shader->ParameterMap(), "Inscatter" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const agl::ShaderParameter& DeltaSRLut() const { return m_deltaSRLut; }
		const agl::ShaderParameter& DeltaSRLutSampler() const { return m_deltaSRLutSampler; }
		const agl::ShaderParameter& Irradiance() const { return m_inscatter; }

	private:
		ComputeShader* m_shader = nullptr;

		agl::ShaderParameter m_deltaSRLut;
		agl::ShaderParameter m_deltaSRLutSampler;
		agl::ShaderParameter m_inscatter;
	};

	class DrawAtmospherePS
	{
	public:
		DrawAtmospherePS()
		{
			m_shader = static_cast<PixelShader*>( GetGlobalShader<DrawAtmospherePS>()->CompileShader( {} ) );
		}

		PixelShader* Shader() { return m_shader; }

	private:
		PixelShader* m_shader = nullptr;
	};

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
				FullScreenQuadVS().Shader(),
				nullptr,
				DrawAtmospherePS().Shader()
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
		commandList.BindShader( transmittanceCS.Shader()->Resource() );
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
		commandList.BindShader( irradianceOneCS.Shader()->Resource() );
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
		commandList.BindShader( inscatterOneCS.Shader()->Resource() );
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
		commandList.BindShader( copyInscatterOneCS.Shader()->Resource() );
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
			commandList.BindShader( inscatterSCS.Shader()->Resource() );

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
			commandList.BindShader( irradianceNCS.Shader()->Resource() );
			BindShaderParameter( commandList, irradianceNCS.DeltaSRLut(), deltaSRTexture );
			BindShaderParameter( commandList, irradianceNCS.DeltaSRLutSampler(), pointSampler.Resource() );
			BindShaderParameter( commandList, irradianceNCS.DeltaSMLut(), deltaSMTexture );
			BindShaderParameter( commandList, irradianceNCS.DeltaSMLutSampler(), pointSampler.Resource() );
			BindShaderParameter( commandList, irradianceNCS.Irradiance(), deltaETexture );
			SetShaderValue( irradianceNCS.Order(), order );

			commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

			// Compute deltaS
			InscatterNCS inscatterNCS;
			commandList.BindShader( inscatterNCS.Shader()->Resource() );
			BindShaderParameter( commandList, inscatterNCS.DeltaJLut(), deltaJTex );
			BindShaderParameter( commandList, inscatterNCS.DeltaJLutSampler(), pointSampler.Resource() );
			BindShaderParameter( commandList, inscatterNCS.DeltaSR(), deltaSRTexture );

			commandList.Dispatch( INSCATTERN_GROUP_X, INSCATTERN_GROUP_Y, INSCATTERN_GROUP_Z );

			// Add deltaE into irradiance textrue E
			CopyIrradianceCS copyIrradianceCS;
			commandList.BindShader( copyIrradianceCS.Shader()->Resource() );
			BindShaderParameter( commandList, copyIrradianceCS.DeltaELut(), deltaETexture );
			BindShaderParameter( commandList, copyIrradianceCS.DeltaELutSampler(), pointSampler.Resource() );
			BindShaderParameter( commandList, copyIrradianceCS.Irradiance(), irradianceBuffer );

			commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

			// Add deltaS info inscatter texture S
			CopyInscatterNCS copyInscatterNCS;
			commandList.BindShader( copyInscatterNCS.Shader()->Resource() );
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
				.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite | agl::ResourceAccessFlag::CpuRead,
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
				.m_access = agl::ResourceAccessFlag::GpuRead
							| agl::ResourceAccessFlag::GpuWrite
							| agl::ResourceAccessFlag::CpuRead
							| agl::ResourceAccessFlag::CpuWrite,
				.m_bindType = agl::ResourceBindType::None,
				.m_miscFlag = agl::ResourceMisc::None
			};

			agl::RefHandle<agl::Texture> irradianceIntermedicate = agl::Texture::Create( intermediate );
			irradianceIntermedicate->Init();

			auto src = GraphicsInterface().Lock( irradianceReadBack, agl::ResourceLockFlag::Read );
			auto dest = GraphicsInterface().Lock( irradianceIntermedicate, agl::ResourceLockFlag::Write );

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
				.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite | agl::ResourceAccessFlag::CpuRead,
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
				.m_access = agl::ResourceAccessFlag::GpuRead
							| agl::ResourceAccessFlag::GpuWrite
							| agl::ResourceAccessFlag::CpuRead
							| agl::ResourceAccessFlag::CpuWrite,
				.m_bindType = agl::ResourceBindType::None,
				.m_miscFlag = agl::ResourceMisc::Texture3D
			};

			agl::RefHandle<agl::Texture> inscatterIntermedicate = agl::Texture::Create( intermediate );
			inscatterIntermedicate->Init();

			auto src = GraphicsInterface().Lock( inscatterReadBack, agl::ResourceLockFlag::Read );
			auto dest = GraphicsInterface().Lock( inscatterIntermedicate, agl::ResourceLockFlag::Write );

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
