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
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<TransmittanceCS>() );

			if ( m_shader )
			{
				m_transmittance.Bind( m_shader->ParameterMap(), "Transmittance" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const aga::ShaderParameter& TransmittanceLut() const { return m_transmittance; }

	private:
		ComputeShader* m_shader = nullptr;

		aga::ShaderParameter m_transmittance;
	};

	class IrradianceOneCS
	{
	public:
		IrradianceOneCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<IrradianceOneCS>() );

			if ( m_shader )
			{
				m_transmittanceLut.Bind( m_shader->ParameterMap(), "TransmittanceLut" );
				m_transmittanceLutSampler.Bind( m_shader->ParameterMap(), "TransmittanceLutSampler" );
				m_deltaE.Bind( m_shader->ParameterMap(), "DeltaE" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const aga::ShaderParameter& TransmittanceLut() const { return m_transmittanceLut; }
		const aga::ShaderParameter& TransmittanceLutSampler() const { return m_transmittanceLutSampler; }
		const aga::ShaderParameter& DeltaE() const { return m_deltaE; }

	private:
		ComputeShader* m_shader = nullptr;

		aga::ShaderParameter m_transmittanceLut;
		aga::ShaderParameter m_transmittanceLutSampler;
		aga::ShaderParameter m_deltaE;
	};

	class InscatterOneCS
	{
	public:
		InscatterOneCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<InscatterOneCS>() );

			if ( m_shader )
			{
				m_transmittanceLut.Bind( m_shader->ParameterMap(), "TransmittanceLut" );
				m_transmittanceLutSampler.Bind( m_shader->ParameterMap(), "TransmittanceLutSampler" );
				m_deltaSR.Bind( m_shader->ParameterMap(), "DeltaSR" );
				m_deltaSM.Bind( m_shader->ParameterMap(), "DeltaSM" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const aga::ShaderParameter& TransmittanceLut() const { return m_transmittanceLut; }
		const aga::ShaderParameter& TransmittanceLutSampler() const { return m_transmittanceLutSampler; }
		const aga::ShaderParameter& DeltaSR() const { return m_deltaSR; }
		const aga::ShaderParameter& DeltaSM() const { return m_deltaSM; }

	private:
		ComputeShader* m_shader = nullptr;

		aga::ShaderParameter m_transmittanceLut;
		aga::ShaderParameter m_transmittanceLutSampler;
		aga::ShaderParameter m_deltaSR;
		aga::ShaderParameter m_deltaSM;
	};

	class CopyInscatterOneCS
	{
	public:
		CopyInscatterOneCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<CopyInscatterOneCS>() );

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
		const aga::ShaderParameter& DeltaSRLut() const { return m_deltaSRLut; }
		const aga::ShaderParameter& DeltaSRLutSampler() const { return m_deltaSRLutSampler; }
		const aga::ShaderParameter& DeltaSMLut() const { return m_deltaSMLut; }
		const aga::ShaderParameter& DeltaSMLutSampler() const { return m_deltaSMLutSampler; }
		const aga::ShaderParameter& Inscatter() const { return m_inscatter; }

	private:
		ComputeShader* m_shader = nullptr;

		aga::ShaderParameter m_deltaSRLut;
		aga::ShaderParameter m_deltaSRLutSampler;
		aga::ShaderParameter m_deltaSMLut;
		aga::ShaderParameter m_deltaSMLutSampler;
		aga::ShaderParameter m_inscatter;
	};

	class InscatterSCS
	{
	public:
		InscatterSCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<InscatterSCS>() );

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
		const aga::ShaderParameter& DeltaELut() const { return m_deltaELut; }
		const aga::ShaderParameter& DeltaELutSampler() const { return m_deltaELutSampler; }
		const aga::ShaderParameter& DeltaSRLut() const { return m_deltaSRLut; }
		const aga::ShaderParameter& DeltaSRLutSampler() const { return m_deltaSRLutSampler; }
		const aga::ShaderParameter& DeltaSMLut() const { return m_deltaSMLut; }
		const aga::ShaderParameter& DeltaSMLutSampler() const { return m_deltaSMLutSampler; }
		const aga::ShaderParameter& Order() const { return m_order; }
		const aga::ShaderParameter& ThreadGroupZ() const { return m_threadGroupZ; }
		const aga::ShaderParameter& DeltaJ() const { return m_deltaJ; }

	private:
		ComputeShader* m_shader = nullptr;

		aga::ShaderParameter m_deltaELut;
		aga::ShaderParameter m_deltaELutSampler;
		aga::ShaderParameter m_deltaSRLut;
		aga::ShaderParameter m_deltaSRLutSampler;
		aga::ShaderParameter m_deltaSMLut;
		aga::ShaderParameter m_deltaSMLutSampler;
		aga::ShaderParameter m_order;
		aga::ShaderParameter m_threadGroupZ;
		aga::ShaderParameter m_deltaJ;
	};

	class IrradianceNCS
	{
	public:
		IrradianceNCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<IrradianceNCS>() );

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
		const aga::ShaderParameter& DeltaSRLut() const { return m_deltaSRLut; }
		const aga::ShaderParameter& DeltaSRLutSampler() const { return m_deltaSRLutSampler; }
		const aga::ShaderParameter& DeltaSMLut() const { return m_deltaSMLut; }
		const aga::ShaderParameter& DeltaSMLutSampler() const { return m_deltaSMLutSampler; }
		const aga::ShaderParameter& Order() const { return m_order; }
		const aga::ShaderParameter& Irradiance() const { return m_irradiance; }

	private:
		ComputeShader* m_shader = nullptr;

		aga::ShaderParameter m_deltaSRLut;
		aga::ShaderParameter m_deltaSRLutSampler;
		aga::ShaderParameter m_deltaSMLut;
		aga::ShaderParameter m_deltaSMLutSampler;
		aga::ShaderParameter m_order;
		aga::ShaderParameter m_irradiance;
	};

	class InscatterNCS
	{
	public:
		InscatterNCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<InscatterNCS>() );

			if ( m_shader )
			{
				m_deltaJLut.Bind( m_shader->ParameterMap(), "DeltaJLut" );
				m_deltaJLutSampler.Bind( m_shader->ParameterMap(), "DeltaJLutSampler" );
				m_deltaSR.Bind( m_shader->ParameterMap(), "DeltaSR" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const aga::ShaderParameter& DeltaJLut() const { return m_deltaJLut; }
		const aga::ShaderParameter& DeltaJLutSampler() const { return m_deltaJLutSampler; }
		const aga::ShaderParameter& DeltaSR() const { return m_deltaSR; }

	private:
		ComputeShader* m_shader = nullptr;

		aga::ShaderParameter m_deltaJLut;
		aga::ShaderParameter m_deltaJLutSampler;
		aga::ShaderParameter m_deltaSR;
	};

	class CopyIrradianceCS
	{
	public:
		CopyIrradianceCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<CopyIrradianceCS>() );

			if ( m_shader )
			{
				m_deltaELut.Bind( m_shader->ParameterMap(), "DeltaELut" );
				m_deltaELutSampler.Bind( m_shader->ParameterMap(), "DeltaELutSampler" );
				m_irradiance.Bind( m_shader->ParameterMap(), "Irradiance" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const aga::ShaderParameter& DeltaELut() const { return m_deltaELut; }
		const aga::ShaderParameter& DeltaELutSampler() const { return m_deltaELutSampler; }
		const aga::ShaderParameter& Irradiance() const { return m_irradiance; }

	private:
		ComputeShader* m_shader = nullptr;

		aga::ShaderParameter m_deltaELut;
		aga::ShaderParameter m_deltaELutSampler;
		aga::ShaderParameter m_irradiance;
	};

	class CopyInscatterNCS
	{
	public:
		CopyInscatterNCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<CopyInscatterNCS>() );

			if ( m_shader )
			{
				m_deltaSRLut.Bind( m_shader->ParameterMap(), "DeltaSRLut" );
				m_deltaSRLutSampler.Bind( m_shader->ParameterMap(), "DeltaSRLutSampler" );
				m_inscatter.Bind( m_shader->ParameterMap(), "Inscatter" );
			}
		}

		ComputeShader* Shader() { return m_shader; }
		const aga::ShaderParameter& DeltaSRLut() const { return m_deltaSRLut; }
		const aga::ShaderParameter& DeltaSRLutSampler() const { return m_deltaSRLutSampler; }
		const aga::ShaderParameter& Irradiance() const { return m_inscatter; }

	private:
		ComputeShader* m_shader = nullptr;

		aga::ShaderParameter m_deltaSRLut;
		aga::ShaderParameter m_deltaSRLutSampler;
		aga::ShaderParameter m_inscatter;
	};

	class DrawAtmospherePS
	{
	public:
		DrawAtmospherePS()
		{
			m_shader = static_cast<PixelShader*>( GetGlobalShader<DrawAtmospherePS>() );
		}

		PixelShader* Shader() { return m_shader; }

	private:
		PixelShader* m_shader = nullptr;
	};

	REGISTER_GLOBAL_SHADER( TransmittanceCS, "./Assets/Shaders/CS_Transmittance.asset" );
	REGISTER_GLOBAL_SHADER( IrradianceOneCS, "./Assets/Shaders/CS_Irradiance1.asset" );
	REGISTER_GLOBAL_SHADER( InscatterOneCS, "./Assets/Shaders/CS_Inscatter1.asset" );
	REGISTER_GLOBAL_SHADER( CopyInscatterOneCS, "./Assets/Shaders/CS_CopyInscatter1.asset" );
	REGISTER_GLOBAL_SHADER( InscatterSCS, "./Assets/Shaders/CS_InscatterS.asset" );
	REGISTER_GLOBAL_SHADER( IrradianceNCS, "./Assets/Shaders/CS_IrradianceN.asset" );
	REGISTER_GLOBAL_SHADER( InscatterNCS, "./Assets/Shaders/CS_InscatterN.asset" );
	REGISTER_GLOBAL_SHADER( CopyIrradianceCS, "./Assets/Shaders/CS_CopyIrradiance.asset" );
	REGISTER_GLOBAL_SHADER( CopyInscatterNCS, "./Assets/Shaders/CS_CopyInscatterN.asset" );
	REGISTER_GLOBAL_SHADER( DrawAtmospherePS, "./Assets/Shaders/PS_DrawAtmosphere.asset" );

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
		rt0BlendOption.m_srcBlend = BLEND::ONE;
		rt0BlendOption.m_destBlend = BLEND::ONE;
		rt0BlendOption.m_srcBlendAlpha = BLEND::ZERO;
		rt0BlendOption.m_destBlendAlpha = BLEND::ONE;

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

		TEXTURE_TRAIT transmittanceLut = {
			.m_width = TRANSMITTANCE_W,
			.m_height = TRANSMITTANCE_H,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = RESOURCE_FORMAT::R32G32B32A32_FLOAT,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			.m_bindType = RESOURCE_BIND_TYPE::SHADER_RESOURCE | RESOURCE_BIND_TYPE::RANDOM_ACCESS,
			.m_miscFlag = 0,
		};

		info.GetTransmittanceLutTexture() = aga::Texture::Create( transmittanceLut );
		EnqueueRenderTask( [texture = info.GetTransmittanceLutTexture()]()
		{
			texture->Init();
		} );

		TEXTURE_TRAIT irradianceLut = {
			.m_width = IRRADIANCE_W,
			.m_height = IRRADIANCE_H,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = RESOURCE_FORMAT::R32G32B32A32_FLOAT,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			.m_bindType = RESOURCE_BIND_TYPE::SHADER_RESOURCE,
			.m_miscFlag = 0,
		};

		info.GetIrradianceLutTexture() = aga::Texture::Create( irradianceLut );
		EnqueueRenderTask( [texture = info.GetIrradianceLutTexture()]()
		{
			texture->Init();
		} );

		TEXTURE_TRAIT inscatterLut = {
			.m_width = RES_MU_S * RES_NU,
			.m_height = RES_MU,
			.m_depth = RES_R,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = RESOURCE_FORMAT::R32G32B32A32_FLOAT,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			.m_bindType = RESOURCE_BIND_TYPE::SHADER_RESOURCE,
			.m_miscFlag = RESOURCE_MISC::TEXTURE3D,
		};

		info.GetInscatterLutTexture() = aga::Texture::Create( inscatterLut );
		EnqueueRenderTask( [texture = info.GetInscatterLutTexture()]()
		{
			texture->Init();
		} );
	}

	void RenderAtmosphereLookUpTables( Scene& scene )
	{
		SkyAtmosphereRenderSceneInfo& info = *scene.SkyAtmosphereSceneInfo();
		if ( info.RebuildLookUpTables() == false )
		{
			return;
		}

		SamplerOption pointSamperOption;
		pointSamperOption.m_filter = TEXTURE_FILTER::POINT;
		auto pointSampler = GraphicsInterface().FindOrCreate( pointSamperOption );

		// 1. Transmittance Table
		TransmittanceCS transmittanceCS;

		auto commandList = rendercore::GetImmediateCommandList();
		commandList.BindShader( transmittanceCS.Shader()->Resource() );
		rendercore::BindShaderParameter( commandList, transmittanceCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );

		commandList.Dispatch( TRANSMITTANCE_GROUP_X, TRANSMITTANCE_GROUP_Y );

		// 2. Ground irradiance due to direct sunlight
		TEXTURE_TRAIT deltaE = {
			.m_width = IRRADIANCE_W,
			.m_height = IRRADIANCE_H,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = RESOURCE_FORMAT::R32G32B32A32_FLOAT,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			.m_bindType = RESOURCE_BIND_TYPE::SHADER_RESOURCE | RESOURCE_BIND_TYPE::RANDOM_ACCESS,
			.m_miscFlag = 0
		};

		RefHandle<aga::Texture> deltaETexture = aga::Texture::Create( deltaE );
		deltaETexture->Init();

		IrradianceOneCS irradianceOneCS;
		commandList.BindShader( irradianceOneCS.Shader()->Resource() );
		rendercore::BindShaderParameter( commandList, irradianceOneCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );
		rendercore::BindShaderParameter( commandList, irradianceOneCS.TransmittanceLutSampler(), pointSampler.Resource() );
		rendercore::BindShaderParameter( commandList, irradianceOneCS.DeltaE(), deltaETexture );

		commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

		// 3. Compute single scattering texture deltaS
		TEXTURE_TRAIT deltaS = {
			.m_width = RES_MU_S * RES_NU,
			.m_height = RES_MU,
			.m_depth = RES_R,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = RESOURCE_FORMAT::R32G32B32A32_FLOAT,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			.m_bindType = RESOURCE_BIND_TYPE::SHADER_RESOURCE | RESOURCE_BIND_TYPE::RANDOM_ACCESS,
			.m_miscFlag = RESOURCE_MISC::TEXTURE3D
		};

		RefHandle<aga::Texture> deltaSRTexture = aga::Texture::Create( deltaS );
		deltaSRTexture->Init();

		RefHandle<aga::Texture> deltaSMTexture = aga::Texture::Create( deltaS );
		deltaSMTexture->Init();

		InscatterOneCS inscatterOneCS;
		commandList.BindShader( inscatterOneCS.Shader()->Resource() );
		rendercore::BindShaderParameter( commandList, inscatterOneCS.TransmittanceLut(), info.GetTransmittanceLutTexture() );
		rendercore::BindShaderParameter( commandList, inscatterOneCS.TransmittanceLutSampler(), pointSampler.Resource() );
		rendercore::BindShaderParameter( commandList, inscatterOneCS.DeltaSR(), deltaSRTexture );
		rendercore::BindShaderParameter( commandList, inscatterOneCS.DeltaSM(), deltaSMTexture );

		commandList.Dispatch( INSCATTER1_GROUP_X, INSCATTER1_GROUP_Y, INSCATTER1_GROUP_Z );

		// 4. Copy deltaS into inscatter texture S
		BUFFER_TRAIT inscatter = {
			.m_stride = sizeof( Vector4 ),
			.m_count = RES_MU_S * RES_NU * RES_MU * RES_R,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			.m_bindType = RESOURCE_BIND_TYPE::RANDOM_ACCESS,
			.m_miscFlag = RESOURCE_MISC::BUFFER_STRUCTURED,
			.m_format = RESOURCE_FORMAT::UNKNOWN
		};

		RefHandle<aga::Buffer> inscatterBuffer = aga::Buffer::Create( inscatter );
		inscatterBuffer->Init();

		CopyInscatterOneCS copyInscatterOneCS;
		commandList.BindShader( copyInscatterOneCS.Shader()->Resource() );
		rendercore::BindShaderParameter( commandList, copyInscatterOneCS.DeltaSRLut(), deltaSRTexture );
		rendercore::BindShaderParameter( commandList, copyInscatterOneCS.DeltaSRLutSampler(), pointSampler.Resource() );
		rendercore::BindShaderParameter( commandList, copyInscatterOneCS.DeltaSMLut(), deltaSMTexture );
		rendercore::BindShaderParameter( commandList, copyInscatterOneCS.DeltaSMLutSampler(), pointSampler.Resource() );
		rendercore::BindShaderParameter( commandList, copyInscatterOneCS.Inscatter(), inscatterBuffer );

		commandList.Dispatch( INSCATTER1_GROUP_X, INSCATTER1_GROUP_Y, INSCATTER1_GROUP_Z );

		RefHandle<aga::Texture> deltaJTex = aga::Texture::Create( deltaS );
		deltaJTex->Init();

		// Irradiance
		BUFFER_TRAIT irradiance = {
			.m_stride = sizeof( Vector4 ),
			.m_count = IRRADIANCE_W * IRRADIANCE_H,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			.m_bindType = RESOURCE_BIND_TYPE::RANDOM_ACCESS,
			.m_miscFlag = RESOURCE_MISC::BUFFER_STRUCTURED,
			.m_format = RESOURCE_FORMAT::UNKNOWN
		};

		Vector4 irradianceData[IRRADIANCE_W * IRRADIANCE_H] = {};
		RefHandle<aga::Buffer> irradianceBuffer = aga::Buffer::Create( irradiance, irradianceData );
		irradianceBuffer->Init();

		// loop for each scattering order
		for ( uint32 order = 2; order <= 4; ++order )
		{
			// Compute deltaJ
			InscatterSCS inscatterSCS;
			commandList.BindShader( inscatterSCS.Shader()->Resource() );

			rendercore::BindShaderParameter( commandList, inscatterSCS.DeltaELut(), deltaETexture );
			rendercore::BindShaderParameter( commandList, inscatterSCS.DeltaELutSampler(), pointSampler.Resource() );
			rendercore::BindShaderParameter( commandList, inscatterSCS.DeltaSRLut(), deltaSRTexture );
			rendercore::BindShaderParameter( commandList, inscatterSCS.DeltaSRLutSampler(), pointSampler.Resource() );
			rendercore::BindShaderParameter( commandList, inscatterSCS.DeltaSMLut(), deltaSMTexture );
			rendercore::BindShaderParameter( commandList, inscatterSCS.DeltaSMLutSampler(), pointSampler.Resource() );
			rendercore::BindShaderParameter( commandList, inscatterSCS.DeltaJ(), deltaJTex );

			for ( uint32 i = 0; i < INSCATTERS_GROUP_Z; ++i )
			{
				rendercore::SetShaderValue( inscatterSCS.Order(), order );
				rendercore::SetShaderValue( inscatterSCS.ThreadGroupZ(), i );

				commandList.Dispatch( INSCATTERS_GROUP_X, INSCATTERS_GROUP_Y );
				commandList.WaitUntilFlush();
			}

			// Compute deltaE
			IrradianceNCS irradianceNCS;
			commandList.BindShader( irradianceNCS.Shader()->Resource() );
			rendercore::BindShaderParameter( commandList, irradianceNCS.DeltaSRLut(), deltaSRTexture );
			rendercore::BindShaderParameter( commandList, irradianceNCS.DeltaSRLutSampler(), pointSampler.Resource() );
			rendercore::BindShaderParameter( commandList, irradianceNCS.DeltaSMLut(), deltaSMTexture );
			rendercore::BindShaderParameter( commandList, irradianceNCS.DeltaSMLutSampler(), pointSampler.Resource() );
			rendercore::BindShaderParameter( commandList, irradianceNCS.Irradiance(), deltaETexture );
			rendercore::SetShaderValue( irradianceNCS.Order(), order );

			commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

			// Compute deltaS
			InscatterNCS inscatterNCS;
			commandList.BindShader( inscatterNCS.Shader()->Resource() );
			rendercore::BindShaderParameter( commandList, inscatterNCS.DeltaJLut(), deltaJTex );
			rendercore::BindShaderParameter( commandList, inscatterNCS.DeltaJLutSampler(), pointSampler.Resource() );
			rendercore::BindShaderParameter( commandList, inscatterNCS.DeltaSR(), deltaSRTexture );

			commandList.Dispatch( INSCATTERN_GROUP_X, INSCATTERN_GROUP_Y, INSCATTERN_GROUP_Z );

			// Add deltaE into irradiance textrue E
			CopyIrradianceCS copyIrradianceCS;
			commandList.BindShader( copyIrradianceCS.Shader()->Resource() );
			rendercore::BindShaderParameter( commandList, copyIrradianceCS.DeltaELut(), deltaETexture );
			rendercore::BindShaderParameter( commandList, copyIrradianceCS.DeltaELutSampler(), pointSampler.Resource() );
			rendercore::BindShaderParameter( commandList, copyIrradianceCS.Irradiance(), irradianceBuffer );

			commandList.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

			// Add deltaS info inscatter texture S
			CopyInscatterNCS copyInscatterNCS;
			commandList.BindShader( copyInscatterNCS.Shader()->Resource() );
			rendercore::BindShaderParameter( commandList, copyInscatterNCS.DeltaSRLut(), deltaSRTexture );
			rendercore::BindShaderParameter( commandList, copyInscatterNCS.DeltaSRLutSampler(), pointSampler.Resource() );
			rendercore::BindShaderParameter( commandList, copyInscatterNCS.Irradiance(), inscatterBuffer );

			commandList.Dispatch( INSCATTERN_GROUP_X, INSCATTERN_GROUP_Y, INSCATTERN_GROUP_Z );
		}

		// copy irradiance buffer to texture
		{
			BUFFER_TRAIT readBack = {
			.m_stride = sizeof( Vector4 ),
			.m_count = IRRADIANCE_W * IRRADIANCE_H,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE | RESOURCE_ACCESS_FLAG::CPU_READ,
			.m_bindType = 0,
			.m_miscFlag = 0,
			.m_format = RESOURCE_FORMAT::UNKNOWN
			};

			RefHandle<aga::Buffer> irradianceReadBack = aga::Buffer::Create( readBack );
			irradianceReadBack->Init();

			commandList.CopyResource( irradianceReadBack, irradianceBuffer );

			TEXTURE_TRAIT intermediate = {
			.m_width = IRRADIANCE_W,
			.m_height = IRRADIANCE_H,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = RESOURCE_FORMAT::R32G32B32A32_FLOAT,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ
						| RESOURCE_ACCESS_FLAG::GPU_WRITE
						| RESOURCE_ACCESS_FLAG::CPU_READ
						| RESOURCE_ACCESS_FLAG::CPU_WRITE,
			.m_bindType = 0,
			.m_miscFlag = 0
			};

			RefHandle<aga::Texture> irradianceIntermedicate = aga::Texture::Create( intermediate );
			irradianceIntermedicate->Init();

			auto src = GraphicsInterface().Lock( irradianceReadBack, BUFFER_LOCKFLAG::READ );
			auto dest = GraphicsInterface().Lock( irradianceIntermedicate, BUFFER_LOCKFLAG::WRITE );

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
			BUFFER_TRAIT readBack = {
			.m_stride = sizeof( Vector4 ),
			.m_count = RES_MU_S * RES_NU * RES_MU * RES_R,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE | RESOURCE_ACCESS_FLAG::CPU_READ,
			.m_bindType = 0,
			.m_miscFlag = 0,
			.m_format = RESOURCE_FORMAT::UNKNOWN
			};

			RefHandle<aga::Buffer> inscatterReadBack = aga::Buffer::Create( readBack );
			inscatterReadBack->Init();

			commandList.CopyResource( inscatterReadBack, inscatterBuffer );

			TEXTURE_TRAIT intermediate = {
			.m_width = RES_MU_S * RES_NU,
			.m_height = RES_MU,
			.m_depth = RES_R,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = RESOURCE_FORMAT::R32G32B32A32_FLOAT,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ
						| RESOURCE_ACCESS_FLAG::GPU_WRITE
						| RESOURCE_ACCESS_FLAG::CPU_READ
						| RESOURCE_ACCESS_FLAG::CPU_WRITE,
			.m_bindType = 0,
			.m_miscFlag = RESOURCE_MISC::TEXTURE3D
			};

			RefHandle<aga::Texture> inscatterIntermedicate = aga::Texture::Create( intermediate );
			inscatterIntermedicate->Init();

			auto src = GraphicsInterface().Lock( inscatterReadBack, BUFFER_LOCKFLAG::READ );
			auto dest = GraphicsInterface().Lock( inscatterIntermedicate, BUFFER_LOCKFLAG::WRITE );

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