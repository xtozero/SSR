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
		int32 len = SPrintf( buf, std::extent_v<decltype(buf)>, "%s_%d", name.c_str(), switches.GetId() );

		return Crc64Hash( buf, len );
	}
}

namespace rendercore
{
	agl::ShaderParameterMap UberShader::m_emptyParameterMap;
	agl::ShaderParameterInfo UberShader::m_emptyParameterInfo;

	REGISTER_ASSET( UberShader );
	ShaderBase* UberShader::CompileShader( const StaticShaderSwitches& switches )
	{
/*
* FIX ME
#ifdef _DEBUG
		if ( m_validVariation.find( switches.GetId() ) == std::end( m_validVariation ) )
		{
			assert( false && "Invalid shader variation" );
			return nullptr;
		}
#endif
*/
		auto thisShared = std::reinterpret_pointer_cast<ShaderAsset>( shared_from_this() );

		uint64 shaderHash = ShaderHash( Path().generic_string(), switches );
		ShaderBase* cache = ShaderCache::GetCachedShader( shaderHash );
		if ( cache != nullptr )
		{
			if ( cache->LastWriteTime() == LastWriteTime() )
			{
				cache->SetParent( thisShared );
				return cache;
			}
		}

		BinaryChunk byteCode = ComipeShaderByteCode( switches );
		if ( byteCode.Size() == 0 )
		{
			return nullptr;
		}

		ShaderBase* shader = nullptr;
		switch ( m_type )
		{
		case agl::ShaderType::Vertex:
			shader = new VertexShader( switches, std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::Geometry:
			shader = new GeometryShader( switches, std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::Pixel:
			shader = new PixelShader( switches, std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::Compute:
			shader = new ComputeShader( switches, std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::Mesh:
			shader = new MeshShader( switches, std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::Amplification:
			shader = new AmplificationShader( switches, std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::RayGen:
			shader = new RayGenerationShader( switches, std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::Intersection:
			shader = new IntersectionShader( switches, std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::AnyHit:
			shader = new AnyHitShader( switches, std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::ClosestHit:
			shader = new ClosestHitShader( switches, std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::Miss:
			shader = new MissShader( switches, std::move( byteCode ), shaderHash );
			break;
		case agl::ShaderType::Callable:
			shader = new CallableShader( switches, std::move( byteCode ), shaderHash );
			break;
		default:
			assert( false && "Invalid shader type" );
			return nullptr;
		}

		GraphicsInterface().BuildShaderMetaData( shader->ByteCode(), shader->ParameterMap(), shader->ParameterInfo() );
		shader->CreateShader();
		shader->SetPath( Path() );
		shader->SetLastWriteTime( LastWriteTime() );
		shader->SetParent( thisShared );

		ShaderCache::UpdateCache( shaderHash, shader );
		return shader;
	}

	void UberShader::RecompileShader()
	{
		// Do Nothing
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

	void UberShader::SetEntryPoint( const std::string& entryPoint )
	{
		m_entryPoint = entryPoint;
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

	BinaryChunk UberShader::ComipeShaderByteCode( const StaticShaderSwitches& switches )
	{
		bool bMeshShader = ( m_type == agl::ShaderType::Mesh ) || ( m_type == agl::ShaderType::Amplification );
        if ( bMeshShader && ( GetInterface<agl::IAgl>()->SupportsMeshShader() == false ) )
        {
        	return {};
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

			SPrintf( value, valueBufferSize, "%d", shaderSwitch.m_current );
			size_t offset = std::strlen( value ) + 1;

			assert( ( valueBufferSize - offset ) < 1024 );

			value += offset;
			valueBufferSize -= offset;
		}
		defines.emplace_back( nullptr );
		defines.emplace_back( nullptr );

		return GraphicsInterface().CompieShader( m_shaderCode, defines, m_type, m_entryPoint.c_str() );
	}

	void UberShader::PostLoadImpl()
	{
	}
}
