#include "UberShader.h"

#include "AbstractGraphicsInterface.h"

#include <array>

namespace rendercore
{
	REGISTER_ASSET( UberShader );
	ShaderBase* UberShader::CompileShader( const StaticShaderSwitches& switches )
	{
		auto compiled = m_compiledShader.find( switches.GetID() );
		if ( compiled != std::end( m_compiledShader ) )
		{
			return compiled->second.get();
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

		std::unique_ptr<ShaderBase> shader;
		switch ( m_type )
		{
		case SHADER_TYPE::VS:
			shader = std::make_unique<VertexShader>( std::move( byteCode ) );
			break;
		case SHADER_TYPE::GS:
			shader = std::make_unique<GeometryShader>( std::move( byteCode ) );
			break;
		case SHADER_TYPE::PS:
			shader = std::make_unique<PixelShader>( std::move( byteCode ) );
			break;
		case SHADER_TYPE::CS:
			shader = std::make_unique<ComputeShader>( std::move( byteCode ) );
			break;
		default:
			assert( false && "Invalid shader type" );
			return nullptr;
		}

		GraphicsInterface().BuildShaderMetaData( shader->ByteCode(), shader->ParameterMap(), shader->ParameterInfo());
		shader->CreateShader();

		auto result = m_compiledShader.emplace( switches.GetID(), std::move( shader ) );
		return result.first->second.get();
	}

	const StaticShaderSwitches& UberShader::Switches() const
	{
		return m_switches;
	}

	void UberShader::PostLoadImpl()
	{
	}
}
