#include "Scene/VolumetricCloudSceneInfo.h"

#include "CommandList.h"
#include "GlobalShaders.h"
#include "ShaderParameterMap.h"
#include "ShaderParameterUtils.h"

namespace rendercore
{
	class PerlinWorleyCS : public GlobalShaderCommon<ComputeShader, PerlinWorleyCS>
	{
	public:
		PerlinWorleyCS()
		{
			m_noiseTex.Bind( GetShader()->ParameterMap(), "NoiseTex");
		}

		const agl::ShaderParameter& NoiseTex() const
		{
			return m_noiseTex;
		}

	private:
		agl::ShaderParameter m_noiseTex;
	};

	class WorleyCS : public GlobalShaderCommon<ComputeShader, WorleyCS>
	{
	public:
		WorleyCS()
		{
			m_noiseTex.Bind( GetShader()->ParameterMap(), "NoiseTex");
		}

		const agl::ShaderParameter& NoiseTex() const
		{
			return m_noiseTex;
		}

	private:
		agl::ShaderParameter m_noiseTex;
	};

	class WeatherMapCS : public GlobalShaderCommon<ComputeShader, WeatherMapCS>
	{
	public:
		WeatherMapCS()
		{
			m_weatherTex.Bind( GetShader()->ParameterMap(), "WeatherTex");
		}

		const agl::ShaderParameter& WeatherTex() const
		{
			return m_weatherTex;
		}

	private:
		agl::ShaderParameter m_weatherTex;
	};

	REGISTER_GLOBAL_SHADER( PerlinWorleyCS, "./Assets/Shaders/Cloud/CS_PerlinWorley.asset" );
	REGISTER_GLOBAL_SHADER( WorleyCS, "./Assets/Shaders/Cloud/CS_Worley.asset" );
	REGISTER_GLOBAL_SHADER( WeatherMapCS, "./Assets/Shaders/Cloud/CS_WeatherMap.asset" );

	void VolumetricCloudSceneInfo::CreateRenderData()
	{
		SetupCloudTexture();
		GenerateWeatherMap();
	}

	VolumetricCloudSceneInfo::VolumetricCloudSceneInfo( VolumetricCloudProxy* proxy ) :
		m_cloudProxy( proxy )
	{
	}

	void VolumetricCloudSceneInfo::SetupCloudTexture()
	{
		m_baseCloudShape = CreateCloudTexture( 128 );

		PerlinWorleyCS perlinWorleyCS;
		auto threadGroupCount = static_cast<uint32>( std::ceilf( 128 / 8.f ) );

		auto commandList = GetImmediateCommandList();

		commandList.BindShader( perlinWorleyCS.GetShader()->Resource() );
		BindShaderParameter( commandList, perlinWorleyCS.NoiseTex(), m_baseCloudShape);

		commandList.Dispatch( threadGroupCount, threadGroupCount, threadGroupCount );

		m_detailCloudShape = CreateCloudTexture( 32 );

		WorleyCS worleyCS;
		threadGroupCount = static_cast<uint32>( std::ceilf( 32 / 8.f ) );

		commandList.BindShader( worleyCS.GetShader()->Resource() );
		BindShaderParameter( commandList, worleyCS.NoiseTex(), m_detailCloudShape);

		commandList.Dispatch( threadGroupCount, threadGroupCount, threadGroupCount );
	}

	void VolumetricCloudSceneInfo::GenerateWeatherMap()
	{
		agl::TEXTURE_TRAIT trait = {
			.m_width = 1024,
			.m_height = 1024,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32G32B32A32_FLOAT,
			.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::None,
		};

		m_weatherMap = agl::Texture::Create( trait );
		EnqueueRenderTask(
			[texture = m_weatherMap]()
			{
				texture->Init();
			}
		);

		WeatherMapCS weatherMapCS;

		auto threadGroupCount = static_cast<uint32>( std::ceilf( 1024.f / 8.f ) );

		auto commandList = GetImmediateCommandList();

		commandList.BindShader( weatherMapCS.GetShader()->Resource() );
		BindShaderParameter( commandList, weatherMapCS.WeatherTex(), m_weatherMap);

		commandList.Dispatch( threadGroupCount, threadGroupCount );
	}

	agl::RefHandle<agl::Texture> VolumetricCloudSceneInfo::CreateCloudTexture( uint32 texSize )
	{
		agl::TEXTURE_TRAIT trait = {
			.m_width = texSize,
			.m_height = texSize,
			.m_depth = texSize,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
			.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::Texture3D
		};

		agl::RefHandle<agl::Texture> cloudTex = agl::Texture::Create( trait );
		EnqueueRenderTask(
			[texture = cloudTex]()
			{
				texture->Init();
			}
		);

		return cloudTex;
	}
}
