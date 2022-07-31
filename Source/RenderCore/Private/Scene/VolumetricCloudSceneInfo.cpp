#include "Scene/VolumetricCloudSceneInfo.h"

#include "CommandList.h"
#include "GlobalShaders.h"
#include "ShaderParameterMap.h"
#include "ShaderParameterUtils.h"

namespace rendercore
{
	class PerlinWorleyCS
	{
	public:
		PerlinWorleyCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<PerlinWorleyCS>()->CompileShader( {} ) );

			m_noiseTex.Bind( m_shader->ParameterMap(), "NoiseTex" );
		}

		ComputeShader* Shader() { return m_shader; }

		aga::ShaderParameter m_noiseTex;

	private:
		ComputeShader* m_shader = nullptr;
	};

	class WorleyCS
	{
	public:
		WorleyCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<WorleyCS>()->CompileShader( {} ) );

			m_noiseTex.Bind( m_shader->ParameterMap(), "NoiseTex" );
		}

		ComputeShader* Shader() { return m_shader; }

		aga::ShaderParameter m_noiseTex;

	private:
		ComputeShader* m_shader = nullptr;
	};

	class WeatherMapCS
	{
	public:
		WeatherMapCS()
		{
			m_shader = static_cast<ComputeShader*>( GetGlobalShader<WeatherMapCS>()->CompileShader( {} ) );

			m_weatherTex.Bind( m_shader->ParameterMap(), "WeatherTex" );
		}

		ComputeShader* Shader() { return m_shader; }

		aga::ShaderParameter m_weatherTex;

	private:
		ComputeShader* m_shader = nullptr;
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

		auto commandList = rendercore::GetImmediateCommandList();

		commandList.BindShader( perlinWorleyCS.Shader()->Resource() );
		BindShaderParameter( commandList, perlinWorleyCS.m_noiseTex, m_baseCloudShape );

		commandList.Dispatch( threadGroupCount, threadGroupCount, threadGroupCount );

		m_detailCloudShape = CreateCloudTexture( 32 );

		WorleyCS worleyCS;
		threadGroupCount = static_cast<uint32>( std::ceilf( 32 / 8.f ) );

		commandList.BindShader( worleyCS.Shader()->Resource() );
		BindShaderParameter( commandList, worleyCS.m_noiseTex, m_detailCloudShape );

		commandList.Dispatch( threadGroupCount, threadGroupCount, threadGroupCount );
	}

	void VolumetricCloudSceneInfo::GenerateWeatherMap()
	{
		TEXTURE_TRAIT trait = {
			.m_width = 1024,
			.m_height = 1024,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = RESOURCE_FORMAT::R32G32B32A32_FLOAT,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			.m_bindType = RESOURCE_BIND_TYPE::SHADER_RESOURCE | RESOURCE_BIND_TYPE::RANDOM_ACCESS,
			.m_miscFlag = RESOURCE_MISC::NONE,
		};

		m_weatherMap = aga::Texture::Create( trait );
		EnqueueRenderTask(
			[texture = m_weatherMap]()
			{
				texture->Init();
			}
		);

		WeatherMapCS weatherMapCS;

		auto threadGroupCount = static_cast<uint32>( std::ceilf( 1024.f / 8.f ) );

		auto commandList = rendercore::GetImmediateCommandList();

		commandList.BindShader( weatherMapCS.Shader()->Resource() );
		BindShaderParameter( commandList, weatherMapCS.m_weatherTex, m_weatherMap );

		commandList.Dispatch( threadGroupCount, threadGroupCount );
	}

	RefHandle<aga::Texture> VolumetricCloudSceneInfo::CreateCloudTexture( uint32 texSize )
	{
		TEXTURE_TRAIT trait = {
			.m_width = texSize,
			.m_height = texSize,
			.m_depth = texSize,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = RESOURCE_FORMAT::R8G8B8A8_UNORM,
			.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			.m_bindType = RESOURCE_BIND_TYPE::SHADER_RESOURCE | RESOURCE_BIND_TYPE::RANDOM_ACCESS,
			.m_miscFlag = RESOURCE_MISC::TEXTURE3D
		};

		RefHandle<aga::Texture> cloudTex = aga::Texture::Create( trait );
		EnqueueRenderTask(
			[texture = cloudTex]()
			{
				texture->Init();
			}
		);

		return cloudTex;
	}
}