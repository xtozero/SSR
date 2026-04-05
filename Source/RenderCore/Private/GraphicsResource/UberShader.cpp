#include "UberShader.h"

#include "AbstractGraphicsInterface.h"
#include "CrcHash.h"
#include "ShaderCache.h"

#include <array>

using ::rendercore::IShaderPermutation;

namespace
{
	uint64 ShaderHash( const std::string& name, const IShaderPermutation& permutation )
	{
		char buf[1024] = {};
		int32 len = SPrintf( buf, std::extent_v<decltype(buf)>, "%s_%d", name.c_str(), permutation.GetPermutationId() );

		return Crc64Hash( buf, len );
	}
}

namespace rendercore
{
	agl::ShaderParameterMap UberShader::m_emptyParameterMap;
	agl::ShaderParameterInfo UberShader::m_emptyParameterInfo;

	REGISTER_ASSET( UberShader );
	ShaderBase* UberShader::CompileShader( const IShaderPermutation& permutation )
	{
		auto thisShared = std::reinterpret_pointer_cast<ShaderAsset>( shared_from_this() );

		uint64 shaderHash = ShaderHash( Path().generic_string(), permutation );
		ShaderBase* cache = ShaderCache::GetCachedShader( shaderHash );
		if ( cache != nullptr )
		{
			if ( cache->LastWriteTime() == LastWriteTime() )
			{
				cache->SetPermutationCreateFunc( m_createPermutationFunc );
				cache->SetParent( thisShared );
				return cache;
			}
		}

		BinaryChunk byteCode = ComipeShaderByteCode( permutation );
		if ( byteCode.Size() == 0 )
		{
			return nullptr;
		}

		ShaderBase* shader = nullptr;
		switch ( m_type )
		{
		case agl::ShaderType::Vertex:
			shader = new VertexShader( std::move( byteCode ), shaderHash, permutation.GetPermutationId() );
			break;
		case agl::ShaderType::Geometry:
			shader = new GeometryShader( std::move( byteCode ), shaderHash, permutation.GetPermutationId() );
			break;
		case agl::ShaderType::Pixel:
			shader = new PixelShader( std::move( byteCode ), shaderHash, permutation.GetPermutationId() );
			break;
		case agl::ShaderType::Compute:
			shader = new ComputeShader( std::move( byteCode ), shaderHash, permutation.GetPermutationId() );
			break;
		case agl::ShaderType::Mesh:
			shader = new MeshShader( std::move( byteCode ), shaderHash, permutation.GetPermutationId() );
			break;
		case agl::ShaderType::Amplification:
			shader = new AmplificationShader( std::move( byteCode ), shaderHash, permutation.GetPermutationId() );
			break;
		case agl::ShaderType::RayGen:
			shader = new RayGenerationShader( std::move( byteCode ), shaderHash, permutation.GetPermutationId(), Name( m_entryPoint ) );
			break;
		case agl::ShaderType::Intersection:
			shader = new IntersectionShader( std::move( byteCode ), shaderHash, permutation.GetPermutationId(), Name( m_entryPoint ) );
			break;
		case agl::ShaderType::AnyHit:
			shader = new AnyHitShader( std::move( byteCode ), shaderHash, permutation.GetPermutationId(), Name( m_entryPoint ) );
			break;
		case agl::ShaderType::ClosestHit:
			shader = new ClosestHitShader( std::move( byteCode ), shaderHash, permutation.GetPermutationId(), Name( m_entryPoint ) );
			break;
		case agl::ShaderType::Miss:
			shader = new MissShader( std::move( byteCode ), shaderHash, permutation.GetPermutationId(), Name( m_entryPoint ) );
			break;
		case agl::ShaderType::Callable:
			shader = new CallableShader( std::move( byteCode ), shaderHash, permutation.GetPermutationId(), Name( m_entryPoint ) );
			break;
		default:
			assert( false && "Invalid shader type" );
			return nullptr;
		}

		GraphicsInterface().BuildShaderMetaData( shader->ByteCode(), shader->ParameterMap(), shader->ParameterInfo() );
		shader->SetPermutationCreateFunc( m_createPermutationFunc );
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

	void UberShader::SetShaderDescriptorHandle( uint32 handle )
	{
		m_shaderDescriptorHandle = handle;
	}

	BinaryChunk UberShader::ComipeShaderByteCode( const IShaderPermutation& permutation ) const
	{
		bool bMeshShader = ( m_type == agl::ShaderType::Mesh ) || ( m_type == agl::ShaderType::Amplification );
		if ( bMeshShader && ( GetInterface<agl::IAgl>()->SupportsMeshShader() == false ) )
		{
			return {};
		}

		class PermutationVisitor : public IShaderDefineVisitor
		{
		public:
			virtual void Visit( const char* name, [[maybe_unused]] const wchar_t* nameW, int32 value ) override
			{
				m_defines.emplace_back( name );
				m_defines.emplace_back( m_valueStr );

				SPrintf( m_valueStr, m_valueBufferSize, "%d", value );
				size_t offset = std::strlen( m_valueStr ) + 1;

				assert( ( m_valueBufferSize - offset ) < 1024 );

				m_valueStr += offset;
				m_valueBufferSize -= offset;
			}

			std::vector<const char*> m_defines;
			std::array<char, 1024> m_valueBuffer{ '\0' };

			char* m_valueStr = m_valueBuffer.data();
			size_t m_valueBufferSize = m_valueBuffer.size();
		} visitor;

		permutation.ForEachShaderDefine( visitor );

		visitor.m_defines.emplace_back( nullptr );
		visitor.m_defines.emplace_back( nullptr );

		return GraphicsInterface().CompieShader( m_shaderCode, visitor.m_defines, m_type, m_entryPoint.c_str() );
	}

	void UberShader::PostLoadImpl()
	{
		auto shaderRegistry = GetInterface<IShaderRegistry>();
		if ( const std::vector<ShaderDescriptor>* shaderDescs = shaderRegistry->Find( m_shaderDescriptorHandle ) )
		{
			for ( const ShaderDescriptor& descriptor : *shaderDescs )
			{
				if ( m_type == descriptor.m_type )
				{
					m_createPermutationFunc = descriptor.m_createPermutationFunc;
					break;
				}
			}
		}
	}
}
