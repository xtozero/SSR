#include "UberShader.h"

#include "AbstractGraphicsInterface.h"
#include "CrcHash.h"
#include "ShaderCache.h"

#include <array>

using ::rendercore::StaticShaderSwitches;

namespace
{
	uint64 ShaderHash( const std::string& name, const StaticShaderSwitches& switches )
	{
		char buf[1024] = {};
		int32 len = sprintf_s( buf, "%s_%d", name.c_str(), switches.GetID() );

		return Crc64Hash( buf, len );
	}
}

namespace rendercore
{
	agl::ShaderParameterMap UberShader::m_emptyParameterMap;
	agl::ShaderParameterInfo UberShader::m_emptyParameterInfo;

	REGISTER_ASSET( UberShader );
	StaticShaderSwitches UberShader::GetStaticSwitches() const
	{
		return m_switches;
	}

	ShaderBase* UberShader::CompileShader( const StaticShaderSwitches& switches )
	{
/*
* FIX ME
#ifdef _DEBUG
		if ( m_validVariation.find( switches.GetID() ) == std::end( m_validVariation ) )
		{
			assert( false && "Invalid shader variation" );
			return nullptr;
		}
#endif
*/

		uint64 shaderHash = ShaderHash( m_name, switches );
		ShaderBase* cache = ShaderCache::GetCachedShader( shaderHash );
		if ( cache != nullptr )
		{
			if ( cache->LastWriteTime() == LastWriteTime() )
			{
				return cache;
			}
		}

		std::vector<const char*> defines;
		defines.reserve( ( switches.Configs().size() + 1 ) << 1 );

		std::array<char, 1024> valueBuffer{ '\0' };
		char* value = valueBuffer.data();
		size_t valueBufferSize = valueBuffer.size();

		for ( auto& [name, shaderSwitch] : switches.Configs() )
		{
			if ( shaderSwitch.m_on == false )
			{
				continue;
			}

			defines.emplace_back( name.Str().data() );
			defines.emplace_back( value );

			sprintf_s( value, valueBufferSize, "%d", shaderSwitch.m_current );
			size_t offset = std::strlen( value ) + 1;

			assert( ( valueBufferSize - offset ) < 1024 );

			value += offset;
			valueBufferSize -= offset;
		}
		defines.emplace_back( nullptr );
		defines.emplace_back( nullptr );

		BinaryChunk byteCode = GraphicsInterface().CompieShader( m_shaderCode, defines, m_profile.Str().data() );

		ShaderBase* shader = nullptr;
		switch ( static_cast<agl::ShaderType>( m_type ) )
		{
		case agl::ShaderType::VS:
			shader = new VertexShader( std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::GS:
			shader = new GeometryShader( std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::PS:
			shader = new PixelShader( std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::CS:
			shader = new ComputeShader( std::move( byteCode ), shaderHash );
			break;
		default:
			assert( false && "Invalid shader type" );
			return nullptr;
		}

		GraphicsInterface().BuildShaderMetaData( shader->ByteCode(), shader->ParameterMap(), shader->ParameterInfo() );
		shader->CreateShader();
		shader->SetPath( Path() );
		shader->SetLastWriteTime( LastWriteTime() );

		ShaderCache::UpdateCache( shaderHash, shader );
		return shader;
	}

	agl::ShaderParameterMap& UberShader::ParameterMap()
	{
		return m_emptyParameterMap;
	}

	const agl::ShaderParameterMap& UberShader::ParameterMap() const
	{
		return m_emptyParameterMap;
	}

	agl::ShaderParameterInfo& UberShader::ParameterInfo()
	{
		return m_emptyParameterInfo;
	}

	const agl::ShaderParameterInfo& UberShader::ParameterInfo() const
	{
		return m_emptyParameterInfo;
	}

	void UberShader::SetName( const std::string& name )
	{
		m_name = name;
	}

	void UberShader::SetShaderType( agl::ShaderType type )
	{
		m_type = type;
	}

	void UberShader::SetProfile( Name profile )
	{
		m_profile = profile;
	}

	void UberShader::SetShaderCode( const std::string& shaderCode )
	{
		std::construct_at( &m_shaderCode, static_cast<uint32>( shaderCode.length() ) );
		std::memcpy( m_shaderCode.Data(), shaderCode.data(), shaderCode.length() );
	}

	void UberShader::SetSwitches( const StaticShaderSwitches& switches )
	{
		m_switches = switches;
	}

	void UberShader::AddValidVariation( uint32 id )
	{
		m_validVariation.emplace( id );
	}

	void UberShader::PostLoadImpl()
	{
	}
}
