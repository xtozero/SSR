#include "UberShader.h"

#include "AbstractGraphicsInterface.h"
#include "Crc64Hash.h"
#include "ShaderCache.h"

#include <array>

namespace
{
	uint64 ShaderHash( const std::string& name, const rendercore::StaticShaderSwitches& switches )
	{
		char buf[1024] = {};
		int32 len = sprintf_s( buf, "%s_%d", name.c_str(), switches.GetID() );

		return Crc64Hash( buf, len );
	}
}

namespace rendercore
{
	REGISTER_ASSET( UberShader );
	ShaderBase* UberShader::CompileShader( const StaticShaderSwitches& switches )
	{
#ifdef _DEBUG
		if ( m_validVariation.find( switches.GetID() ) == std::end( m_validVariation ) )
		{
			assert( false && "Invalid shader variation" );
			return nullptr;
		}
#endif

		uint64 shaderHash = ShaderHash( m_name, switches );
		ShaderBase* cache = ShaderCache::GetCachedShader( shaderHash );
		if ( cache != nullptr )
		{
			return cache;
		}

		std::vector<const char*> defines;
		defines.reserve( ( m_switches.Configs().size() + 1 ) << 1 );

		std::array<char, 1024> valueBuffer{ '\0' };
		char* value = valueBuffer.data();
		for ( auto& [name, shaderSwitch] : m_switches.Configs() )
		{
			if ( shaderSwitch.m_on == false )
			{
				continue;
			}

			defines.emplace_back( name.Str().data() );
			defines.emplace_back( value );

			sprintf_s( value, &*std::end( valueBuffer ) - value, "%d", shaderSwitch.m_current);
			value += ( std::strlen( value ) + 1 );
		}
		defines.emplace_back( nullptr );
		defines.emplace_back( nullptr );

		BinaryChunk byteCode = GraphicsInterface().CompieShader( m_shaderCode, defines, m_profile.Str().data() );

		ShaderBase* shader = nullptr;
		switch ( m_type )
		{
		case SHADER_TYPE::VS:
			shader = new VertexShader( std::move( byteCode ) );
			break;
		case SHADER_TYPE::GS:
			shader = new GeometryShader( std::move( byteCode ) );
			break;
		case SHADER_TYPE::PS:
			shader = new PixelShader( std::move( byteCode ) );
			break;
		case SHADER_TYPE::CS:
			shader = new ComputeShader( std::move( byteCode ) );
			break;
		default:
			assert( false && "Invalid shader type" );
			return nullptr;
		}

		GraphicsInterface().BuildShaderMetaData( shader->ByteCode(), shader->ParameterMap(), shader->ParameterInfo());
		shader->CreateShader();

		ShaderCache::UpdateCache( shaderHash, shader );
		return shader;
	}

	const StaticShaderSwitches& UberShader::Switches() const
	{
		return m_switches;
	}

	void UberShader::PostLoadImpl()
	{
	}
}
