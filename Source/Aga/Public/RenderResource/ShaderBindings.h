#pragma once

#include "Buffer.h"
#include "Texture.h"
#include "GraphicsApiResource.h"
#include "IAga.h"
#include "PipelineState.h"
#include "ResourceViews.h"
#include "ShaderParameterInfo.h"
#include "ShaderResource.h"
#include "SizedTypes.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <optional>
#include <vector>

namespace aga
{
	class ShaderBindingLayout
	{
	public:
		size_t GetDataSize() const
		{
			return ( m_parameterInfo.m_constantBuffers.size() + m_parameterInfo.m_srvs.size() + m_parameterInfo.m_uavs.size() + m_parameterInfo.m_samplers.size() ) * sizeof( RefHandle<GraphicsApiResource> );
		}

		const ShaderParameterInfo& ParameterInfo() const
		{
			return m_parameterInfo;
		}

		SHADER_TYPE ShaderType() const
		{
			return m_shaderType;
		}

		size_t GetHash() const
		{
			static size_t typeHash = typeid( ShaderBindingLayout ).hash_code();
			size_t hash = typeHash;

			HashCombine( hash, m_shaderType );
			HashCombine( hash, m_parameterInfo.GetHash() );

			return hash;
		}

		ShaderBindingLayout() = default;
		ShaderBindingLayout( SHADER_TYPE type, const ShaderParameterInfo& parameterInfo ) : m_shaderType( type ), m_parameterInfo( parameterInfo ) {}
		ShaderBindingLayout( const ShaderBindingLayout& other )
		{
			( *this ) = other;
		}
		ShaderBindingLayout& operator=( const ShaderBindingLayout& other )
		{
			if ( this != &other )
			{
				m_shaderType = other.m_shaderType;
				m_parameterInfo = other.m_parameterInfo;
			}

			return *this;
		}

		ShaderBindingLayout( ShaderBindingLayout&& other ) noexcept
		{
			( *this ) = std::move( other );
		}
		ShaderBindingLayout& operator=( ShaderBindingLayout&& other ) noexcept
		{
			if ( this != &other )
			{
				m_shaderType = other.m_shaderType;
				m_parameterInfo = std::move( other.m_parameterInfo );
			}

			return *this;
		}

		friend bool operator==( const ShaderBindingLayout& lhs, const ShaderBindingLayout& rhs )
		{
			return lhs.m_shaderType == rhs.m_shaderType &&
				lhs.m_parameterInfo == rhs.m_parameterInfo;
		}

		friend bool operator!=( const ShaderBindingLayout& lhs, const ShaderBindingLayout& rhs )
		{
			return !( lhs == rhs );
		}

	protected:
		size_t GetConstantBufferOffset() const
		{
			return 0;
		}

		size_t GetSRVOffset() const
		{
			return m_parameterInfo.m_constantBuffers.size() * sizeof( RefHandle<GraphicsApiResource> );
		}

		size_t GetUAVOffset() const
		{
			return GetSRVOffset() + m_parameterInfo.m_srvs.size() * sizeof( RefHandle<GraphicsApiResource> );
		}

		size_t GetSamplerOffset() const
		{
			return GetUAVOffset() + m_parameterInfo.m_uavs.size() * sizeof( RefHandle<GraphicsApiResource> );
		}

		SHADER_TYPE m_shaderType = SHADER_TYPE::NONE;
		ShaderParameterInfo m_parameterInfo;
	};

	class SingleShaderBindings : public ShaderBindingLayout
	{
	public:
		void AddConstantBuffer( const ShaderParameter& param, Buffer* buffer )
		{
			if ( param.m_shader != m_shaderType )
			{
				return;
			}

			std::optional<uint32> foundSlot;

			for ( size_t i = 0; i < m_parameterInfo.m_constantBuffers.size(); ++i )
			{
				if ( m_parameterInfo.m_constantBuffers[i].m_bindPoint == param.m_bindPoint )
				{
					foundSlot = static_cast<uint32>( i );
				}
			}

			if ( foundSlot )
			{
				GetConstantBufferStart()[*foundSlot] = buffer;
			}
		}

		void AddSRV( const ShaderParameter& param, ShaderResourceView* srv )
		{
			if ( param.m_shader != m_shaderType )
			{
				return;
			}

			std::optional<uint32> foundSlot;

			for ( size_t i = 0; i < m_parameterInfo.m_srvs.size(); ++i )
			{
				if ( m_parameterInfo.m_srvs[i].m_bindPoint == param.m_bindPoint )
				{
					foundSlot = static_cast<uint32>( i );
				}
			}

			if ( foundSlot )
			{
				GetSRVStart()[*foundSlot] = srv;
			}
		}

		void AddUAV( const ShaderParameter& param, UnorderedAccessView* uav )
		{
			if ( param.m_shader != m_shaderType )
			{
				return;
			}

			std::optional<uint32> foundSlot;

			for ( size_t i = 0; i < m_parameterInfo.m_uavs.size(); ++i )
			{
				if ( m_parameterInfo.m_uavs[i].m_bindPoint == param.m_bindPoint )
				{
					foundSlot = static_cast<uint32>( i );
				}
			}

			if ( foundSlot )
			{
				GetUAVStart()[*foundSlot] = uav;
			}
		}

		void AddSampler( const ShaderParameter& param, SamplerState* sampler )
		{
			if ( param.m_shader != m_shaderType )
			{
				return;
			}

			std::optional<uint32> foundSlot;

			for ( size_t i = 0; i < m_parameterInfo.m_samplers.size(); ++i )
			{
				if ( m_parameterInfo.m_samplers[i].m_bindPoint == param.m_bindPoint )
				{
					foundSlot = static_cast<uint32>( i );
				}
			}

			if ( foundSlot )
			{
				GetSamplerStart()[*foundSlot] = sampler;
			}
		}

		RefHandle<GraphicsApiResource>* GetConstantBufferStart() const
		{
			return reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + GetConstantBufferOffset() );
		}

		RefHandle<GraphicsApiResource>* GetSRVStart() const
		{
			return reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + GetSRVOffset() );
		}

		RefHandle<GraphicsApiResource>* GetUAVStart() const
		{
			return reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + GetUAVOffset() );
		}

		RefHandle<GraphicsApiResource>* GetSamplerStart() const
		{
			return reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + GetSamplerOffset() );
		}

		SingleShaderBindings( const ShaderBindingLayout& bindingLayout, unsigned char* data ) : ShaderBindingLayout( bindingLayout ), m_data( data ) {}

	private:
		unsigned char* m_data = nullptr;
	};

	class ShaderBindingsInitializer
	{
	public:
		size_t NumShaders() const
		{
			auto pred = []( const ShaderParameterInfo* info )
			{
				return info != nullptr;
			};

			return std::count_if( std::begin( m_shaderParameterInfos ), std::end( m_shaderParameterInfos ), pred );
		}

		const ShaderParameterInfo*& operator[]( SHADER_TYPE type )
		{
			assert( SHADER_TYPE::NONE < type&& type < SHADER_TYPE::Count );
			return m_shaderParameterInfos[static_cast<uint32>( type )];
		}

		const ShaderParameterInfo* operator[]( SHADER_TYPE type ) const
		{
			assert( SHADER_TYPE::NONE < type&& type < SHADER_TYPE::Count );
			return m_shaderParameterInfos[static_cast<uint32>( type )];
		}

	private:
		const ShaderParameterInfo* m_shaderParameterInfos[MAX_SHADER_TYPE<uint32>] = {};
	};

	class ShaderBindings
	{
	public:
		void Initialize( const ShaderBindingsInitializer& initializer )
		{
			size_t numShaders = initializer.NumShaders();

			size_t shaderBindsDataSize = 0;
			m_shaderLayouts = new ShaderBindingLayout[numShaders];

			for ( uint32 i = 0; i < MAX_SHADER_TYPE<uint32>; ++i )
			{
				auto shaderType = static_cast<SHADER_TYPE>( i );
				if ( initializer[shaderType] )
				{
					new ( &m_shaderLayouts[m_shaderLayoutSize] )ShaderBindingLayout( shaderType, *initializer[shaderType] );
					shaderBindsDataSize += m_shaderLayouts[m_shaderLayoutSize].GetDataSize();
					++m_shaderLayoutSize;
				}
			}

			Allocate( shaderBindsDataSize );
		}

		SingleShaderBindings GetSingleShaderBindings( SHADER_TYPE shaderType ) const
		{
			size_t dataOffset = 0;
			for ( uint32 i = 0; i < m_shaderLayoutSize; ++i )
			{
				if ( m_shaderLayouts[i].ShaderType() == shaderType )
				{
					return SingleShaderBindings( m_shaderLayouts[i], m_data + dataOffset );
				}

				dataOffset += m_shaderLayouts[i].GetDataSize();
			}

			static ShaderParameterInfo emptyParameterInfo;
			static ShaderBindingLayout emptyLayout( SHADER_TYPE::NONE, emptyParameterInfo );
			return SingleShaderBindings( emptyLayout, nullptr );
		}

		ShaderBindings() = default;
		ShaderBindings( const ShaderBindings& other )
		{
			( *this ) = other;
		}
		ShaderBindings& operator=( const ShaderBindings& other )
		{
			if ( this != &other )
			{
				Free();

				m_shaderLayouts = new ShaderBindingLayout[other.m_shaderLayoutSize];
				m_shaderLayoutSize = other.m_shaderLayoutSize;
				std::copy_n( other.m_shaderLayouts, m_shaderLayoutSize, m_shaderLayouts );
				Allocate( other.m_size );

				for ( size_t offset = 0; offset < m_size; offset += sizeof( RefHandle<GraphicsApiResource> ) )
				{
					RefHandle<GraphicsApiResource>* handle = reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + offset );
					RefHandle<GraphicsApiResource>* otherHandle = reinterpret_cast<RefHandle<GraphicsApiResource>*>( other.m_data + offset );

					*handle = *otherHandle;
				}
			}

			return *this;
		}

		ShaderBindings( ShaderBindings&& other ) noexcept
		{
			( *this ) = std::move( other );
		}
		ShaderBindings& operator=( ShaderBindings&& other ) noexcept
		{
			if ( this != &other )
			{
				Free();

				m_shaderLayouts = other.m_shaderLayouts;
				m_shaderLayoutSize = other.m_shaderLayoutSize;
				m_data = other.m_data;
				m_size = other.m_size;

				other.m_shaderLayouts = nullptr;
				other.m_shaderLayoutSize = 0;
				other.m_data = nullptr;
				other.m_size = 0;
			}

			return *this;
		}

		~ShaderBindings()
		{
			Free();
		}

		bool MatchsForDynamicInstancing( const ShaderBindings& other ) const
		{
			if ( ( m_shaderLayoutSize != other.m_shaderLayoutSize ) ||
				( m_size != other.m_size ) )
			{
				return false;
			}

			for ( uint32 i = 0; i < m_shaderLayoutSize; ++i )
			{
				if ( m_shaderLayouts[i] != other.m_shaderLayouts[i] )
				{
					return false;
				}
			}

			return std::memcmp( m_data, other.m_data, m_size ) == 0;
		}

		size_t HashForDynamicInstaning() const
		{
			static size_t typeHash = typeid( ShaderBindings ).hash_code();
			size_t hash = typeHash;

			HashCombine( hash, m_size );

			for ( uint32 i = 0; i < m_shaderLayoutSize; ++i )
			{
				HashCombine( hash, m_shaderLayouts[i].GetHash() );
			}

			for ( size_t offset = 0; offset < m_size; offset += sizeof( RefHandle<GraphicsApiResource> ) )
			{
				auto handle = reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + offset );
				HashCombine( hash, handle->Get() );
			}

			return hash;
		}

	private:
		void Allocate( size_t size )
		{
			if ( size > 0 )
			{
				assert( m_data == nullptr && m_size == 0 );
				assert( size % sizeof( RefHandle<GraphicsApiResource> ) == 0 );
				m_data = new unsigned char[size];
				m_size = size;

				for ( size_t offset = 0; offset < m_size; offset += sizeof( RefHandle<GraphicsApiResource> ) )
				{
					RefHandle<GraphicsApiResource>* handle = reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + offset );
					std::construct_at( handle );
				}
			}
		}

		void Free()
		{
			delete[] m_shaderLayouts;
			m_shaderLayouts = nullptr;
			m_shaderLayoutSize = 0;

			if ( m_size > 0 )
			{
				for ( size_t offset = 0; offset < m_size; offset += sizeof( RefHandle<GraphicsApiResource> ) )
				{
					auto handle = reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + offset );
					handle->~RefHandle<GraphicsApiResource>();
				}
			}

			delete[] m_data;
			m_data = nullptr;
			m_size = 0;
		}

		ShaderBindingLayout* m_shaderLayouts = nullptr;
		uint32 m_shaderLayoutSize = 0;

		unsigned char* m_data = nullptr;
		size_t m_size = 0;
	};
}