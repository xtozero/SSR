#include "ShaderArguments.h"

namespace rendercore
{
	Name ShaderArgumentsMetaData::GetName() const
	{
		return m_name;
	}

	uint32 ShaderArgumentsMetaData::GetConstantBufferSize() const
	{
		return m_constantBufferSize;
	}

	const std::vector<ShaderArgumentsMetaData::ShaderValueLayout>& ShaderArgumentsMetaData::GetShaderValueLayouts() const
	{
		return m_shaderValueLayouts;
	}

	const std::vector<ShaderArgumentsMetaData::ShaderResourceLayout>& ShaderArgumentsMetaData::GetShaderResourceLayouts() const
	{
		return m_shaderResourceLayouts;
	}

	ShaderArgumentsMetaData::ShaderArgumentsMetaData( Name name,
		const ShaderArgumentsMemberMetaData* firstMemberMetaData )
		: m_name( name )
	{
		int32 numShaderValue = 0;
		int32 numShaderResource = 0;

		const ShaderArgumentsMemberMetaData* iter = firstMemberMetaData;
		while ( iter != nullptr )
		{
			const auto& memberMetaData = *iter;
			iter = iter->m_next;

			if ( memberMetaData.m_type == ShaderArgumentsMemberType::ShaderValue )
			{
				++numShaderValue;
			}
			else
			{
				++numShaderResource;
			}
		}

		m_shaderValueLayouts.reserve( numShaderValue );
		m_shaderResourceLayouts.reserve( numShaderResource );

		iter = firstMemberMetaData;
		while ( iter != nullptr )
		{
			const auto& memberMetaData = *iter;
			iter = iter->m_next;

			if ( memberMetaData.m_type == ShaderArgumentsMemberType::ShaderValue )
			{
				uint32 alignedSize = CalcAlignment<uint32>( m_constantBufferSize, sizeof( float4 ) );
				if ( alignedSize < ( m_constantBufferSize + memberMetaData.m_size ) )
				{
					m_constantBufferSize = alignedSize;
				}

				m_shaderValueLayouts.emplace_back( &memberMetaData, m_constantBufferSize );

				m_constantBufferSize += memberMetaData.m_size;
			}
			else
			{
				m_shaderResourceLayouts.emplace_back( &memberMetaData );
			}
		}

		m_constantBufferSize = CalcAlignment<uint32>( m_constantBufferSize, sizeof( float4 ) );
	}

	void ShaderArguments::Bind( const agl::ShaderParameterMap& parameterMap, agl::SingleShaderBindings& singleBinding ) const
	{
		if ( m_buffer.Get() != nullptr )
		{
			agl::ShaderParameter parameter = parameterMap.GetParameter( m_metaData.GetName() );
			if ( parameter.m_type == agl::ShaderParameterType::ConstantBuffer )
			{
				singleBinding.AddConstantBuffer( parameter, m_buffer.Get() );
			}
		}

		const std::vector<ShaderArgumentsMetaData::ShaderResourceLayout>& resourceLayouts = m_metaData.GetShaderResourceLayouts();
		for ( size_t i = 0; i < m_resources.size(); ++i )
		{
			const ShaderArgumentsMetaData::ShaderResourceLayout& layout = resourceLayouts[i];

			agl::ShaderParameter parameter = parameterMap.GetParameter( layout.m_memberMetaData->m_name );
			switch ( parameter.m_type )
			{
			case agl::ShaderParameterType::SRV:
				singleBinding.AddSRV( parameter, reinterpret_cast<agl::ShaderResourceView*>( m_resources[i].Get() ) );
				break;
			case agl::ShaderParameterType::UAV:
				singleBinding.AddUAV( parameter, reinterpret_cast<agl::UnorderedAccessView*>( m_resources[i].Get() ) );
				break;
			case agl::ShaderParameterType::Sampler:
				singleBinding.AddSampler( parameter, reinterpret_cast<agl::SamplerState*>( m_resources[i].Get() ) );
				break;
			case agl::ShaderParameterType::Bindless:
				singleBinding.AddBindless( parameter, m_resources[i].Get() );
				break;
			default:
				break;
			}
		}
	}

	agl::Buffer* ShaderArguments::Resource()
	{
		return m_buffer.Get();
	}

	const agl::Buffer* ShaderArguments::Resource() const
	{
		return m_buffer.Get();
	}

	ShaderArguments::ShaderArguments( const ShaderArgumentsMetaData& metaData )
		: m_metaData( metaData )
	{
		InitResource( nullptr );
	}

	void ShaderArguments::Finalizer()
	{
		delete this;
	}

	void ShaderArguments::InitResource( void* contents )
	{
		m_resources.resize( m_metaData.GetShaderResourceLayouts().size() );
		
		uint32 constantBufferSize = m_metaData.GetConstantBufferSize();
		thread_local std::vector<uint8> initData;
		initData.resize( constantBufferSize );

		if ( contents )
		{
			UpdateShaderValue( initData.data(), contents );
			UpdateShaderResouce( contents );
		}

		if ( constantBufferSize > 0 )
		{
			agl::BufferTrait trait = {
			   .m_stride = constantBufferSize,
			   .m_count = 1,
			   .m_access = agl::ResourceAccessFlag::Upload,
			   .m_bindType = agl::ResourceBindType::ConstantBuffer,
			   .m_miscFlag = agl::ResourceMisc::None,
			   .m_format = agl::ResourceFormat::Unknown
			};

			m_buffer = agl::Buffer::Create( trait, m_metaData.GetName().CStr(), initData.data() );
			EnqueueRenderTask(
				[buffer = m_buffer]()
				{
					buffer->Init();
				} );
		}
	}

	void ShaderArguments::Update( void* contents )
	{
		assert( contents != nullptr );
		assert( IsInRenderThread() );

		if ( m_buffer.Get() == nullptr )
		{
			InitResource( contents );
		}
		else
		{
			auto data = static_cast<uint8*>( GraphicsInterface().Lock( m_buffer ).m_data );
			UpdateShaderValue( data, contents );
			GraphicsInterface().UnLock( m_buffer );

			UpdateShaderResouce( contents );
		}
	}

	void ShaderArguments::UpdateShaderValue( uint8* dest, void* contents )
	{
		if ( m_metaData.GetConstantBufferSize() == 0 )
		{
			return;
		}

		assert( dest != nullptr );
		assert( contents != nullptr );

		const std::vector<ShaderArgumentsMetaData::ShaderValueLayout>& valueLayouts = m_metaData.GetShaderValueLayouts();
		for ( const ShaderArgumentsMetaData::ShaderValueLayout& layout : valueLayouts )
		{
			uint8* copyDest = dest + layout.m_offsetFromBufferStart;
			const uint8* copySrc = static_cast<const uint8*>( contents ) + layout.m_memberMetaData->m_offset;

			std::memcpy( copyDest, copySrc, layout.m_memberMetaData->m_size );
		}
	}

	void ShaderArguments::UpdateShaderResouce( void* contents )
	{
		assert( contents != nullptr );

		const std::vector<ShaderArgumentsMetaData::ShaderResourceLayout>& resourceLayouts = m_metaData.GetShaderResourceLayouts();
		for ( size_t i = 0; i < m_resources.size(); ++i )
		{
			const ShaderArgumentsMetaData::ShaderResourceLayout& layout = resourceLayouts[i];
			uint8* resource = static_cast<uint8*>( contents ) + layout.m_memberMetaData->m_offset;

			m_resources[i] = *reinterpret_cast<RefHandle<agl::GraphicsApiResource>*>( resource );
		}
	}
}
