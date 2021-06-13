#pragma once

#include "Buffer.h"
#include "Texture.h"
#include "GraphicsApiResource.h"
#include "IAga.h"
#include "PipelineState.h"
#include "ResourceViews.h"
#include "ShaderPrameterInfo.h"
#include "ShaderResource.h"

#include <cassert>
#include <optional>
#include <vector>

namespace aga
{
	class ShaderBindingLayout
	{
	public:
		std::size_t GetDataSize( ) const
		{
			return ( m_parameterInfo.m_constantBuffers.size( ) + m_parameterInfo.m_srvs.size( ) + m_parameterInfo.m_uavs.size( ) + m_parameterInfo.m_samplers.size( ) ) * sizeof( RefHandle<GraphicsApiResource> );
		}

		const ShaderParameterInfo& ParameterInfo( ) const
		{
			return m_parameterInfo;
		}

		SHADER_TYPE ShaderType( ) const
		{
			return m_shaderType;
		}

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

	protected:
		std::size_t GetConstantBufferOffset( ) const
		{
			return 0;
		}

		std::size_t GetSRVOffset( ) const
		{
			return m_parameterInfo.m_srvs.size( ) * sizeof( RefHandle<GraphicsApiResource> );
		}

		std::size_t GetUAVOffset( ) const
		{
			return GetSRVOffset( ) + m_parameterInfo.m_srvs.size( ) * sizeof( RefHandle<GraphicsApiResource> );
		}

		std::size_t GetSamplerOffset( ) const
		{
			return GetUAVOffset( ) + m_parameterInfo.m_uavs.size( ) * sizeof( RefHandle<GraphicsApiResource> );
		}

		SHADER_TYPE m_shaderType = SHADER_TYPE::NONE;
		ShaderParameterInfo m_parameterInfo;
	};

	class SingleShaderBindings : public ShaderBindingLayout
	{
	public:
		void AddConstantBuffer( UINT slot, Buffer* buffer )
		{
			std::optional<UINT> foundSlot;

			for ( std::size_t i = 0; i < m_parameterInfo.m_constantBuffers.size( ); ++i )
			{
				if ( m_parameterInfo.m_constantBuffers[i].m_bindPoint == slot )
				{
					foundSlot = static_cast<UINT>( i );
				}
			}

			if ( foundSlot )
			{
				GetConstantBufferStart( )[*foundSlot] = buffer;
			}
		}

		void AddSRV( UINT slot, ShaderResourceView* srv )
		{
			std::optional<UINT> foundSlot;

			for ( std::size_t i = 0; i < m_parameterInfo.m_srvs.size( ); ++i )
			{
				if ( m_parameterInfo.m_srvs[i].m_bindPoint == slot )
				{
					foundSlot = static_cast<UINT>( i );
				}
			}

			if ( foundSlot )
			{
				GetSRVStart( )[*foundSlot] = srv;
			}
		}

		void AddUAV( UINT slot, UnorderedAccessView* uav )
		{
			std::optional<UINT> foundSlot;

			for ( std::size_t i = 0; i < m_parameterInfo.m_uavs.size( ); ++i )
			{
				if ( m_parameterInfo.m_uavs[i].m_bindPoint == slot )
				{
					foundSlot = static_cast<UINT>( i );
				}
			}

			if ( foundSlot )
			{
				GetUAVStart( )[*foundSlot] = uav;
			}
		}

		void AddSampler( UINT slot, SamplerState* sampler )
		{
			std::optional<UINT> foundSlot;

			for ( std::size_t i = 0; i < m_parameterInfo.m_samplers.size( ); ++i )
			{
				if ( m_parameterInfo.m_samplers[i].m_bindPoint == slot )
				{
					foundSlot = static_cast<UINT>( i );
				}
			}

			if ( foundSlot )
			{
				GetSamplerStart( )[*foundSlot] = sampler;
			}
		}

		RefHandle<GraphicsApiResource>* GetConstantBufferStart( ) const
		{
			return reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + GetConstantBufferOffset( ) );
		}

		RefHandle<GraphicsApiResource>* GetSRVStart( ) const
		{
			return reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + GetSRVOffset( ) );
		}

		RefHandle<GraphicsApiResource>* GetUAVStart( ) const
		{
			return reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + GetUAVOffset( ) );
		}

		RefHandle<GraphicsApiResource>* GetSamplerStart( ) const
		{
			return reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + GetSamplerOffset( ) );
		}

		SingleShaderBindings( const ShaderBindingLayout& bindingLayout, unsigned char* data ) : ShaderBindingLayout( bindingLayout ), m_data( data ) {}

	private:
		unsigned char* m_data = nullptr;
	};

	class ShaderBindingsInitializer
	{
	public:
		const ShaderParameterInfo* m_shaderParameterInfos[MAX_SHADER_TYPE<int>] = {};

		const ShaderParameterInfo*& operator[]( SHADER_TYPE type )
		{
			assert( SHADER_TYPE::NONE < type && type < SHADER_TYPE::Count );
			return m_shaderParameterInfos[static_cast<int>( type )];
		}

		const ShaderParameterInfo* operator[]( SHADER_TYPE type ) const
		{
			assert( SHADER_TYPE::NONE < type && type < SHADER_TYPE::Count );
			return m_shaderParameterInfos[static_cast<int>( type )];
		}
	};

	class ShaderBindings
	{
	public:
		void Initialize( const ShaderBindingsInitializer& initializer )
		{
			std::size_t numShaders = ( initializer[SHADER_TYPE::VS] ? 1 : 0 ) +
				//( shaders.m_hullShader.IsValid( ) ? 1 : 0 ) +
				//( shaders.m_domainShader.IsValid( ) ? 1 : 0 ) +
				//( shaders.m_geometryShader.IsValid( ) ? 1 : 0 ) +
				( initializer[SHADER_TYPE::PS] ? 1 : 0 );

			m_shaderLayouts.reserve( numShaders );
			std::size_t shaderBindsDataSize = 0;

			if ( initializer[SHADER_TYPE::VS] )
			{
				m_shaderLayouts.emplace_back( SHADER_TYPE::VS, *initializer[SHADER_TYPE::VS] );
				shaderBindsDataSize += m_shaderLayouts.back( ).GetDataSize( );
			}

			//if ( shaders.m_hullShader.IsValid( ) )
			//{
			//	m_shaderLayouts.emplace_back( aga.GetShaderParameterInfo( shaders.m_hullShader ) );
			//	shaderBindsDataSize += m_shaderLayouts.back( ).GetDataSize( );
			//}

			//if ( shaders.m_domainShader.IsValid( ) )
			//{
			//	m_shaderLayouts.emplace_back( aga.GetShaderParameterInfo( shaders.m_domainShader ) );
			//	shaderBindsDataSize += m_shaderLayouts.back( ).GetDataSize( );
			//}

			//if ( shaders.m_geometryShader.IsValid( ) )
			//{
			//	m_shaderLayouts.emplace_back( aga.GetShaderParameterInfo( shaders.m_geometryShader ) );
			//	shaderBindsDataSize += m_shaderLayouts.back( ).GetDataSize( );
			//}

			if ( initializer[SHADER_TYPE::PS] )
			{
				m_shaderLayouts.emplace_back( SHADER_TYPE::PS, *initializer[SHADER_TYPE::PS] );
				shaderBindsDataSize += m_shaderLayouts.back( ).GetDataSize( );
			}

			Allocate( shaderBindsDataSize );
		}

		SingleShaderBindings GetSingleShaderBindings( SHADER_TYPE shaderType ) const
		{
			for ( auto iter = m_shaderLayouts.begin( ); iter != m_shaderLayouts.end( ); ++iter )
			{
				std::size_t dataOffset = 0;

				if ( iter->ShaderType( ) == shaderType )
				{
					return SingleShaderBindings( *iter, m_data + dataOffset );
				}

				dataOffset += iter->GetDataSize( );
			}

			static ShaderParameterInfo emptyParameterInfo;
			static ShaderBindingLayout emptyLayout( SHADER_TYPE::NONE, emptyParameterInfo );
			return SingleShaderBindings( emptyLayout, nullptr );
		}

		ShaderBindings( ) = default;
		ShaderBindings( const ShaderBindings& other )
		{
			( *this ) = other;
		}
		ShaderBindings& operator=( const ShaderBindings& other )
		{
			if ( this != &other )
			{
				Free( );

				m_shaderLayouts = other.m_shaderLayouts;
				Allocate( other.m_size );

				std::memcpy( m_data, other.m_data, m_size );
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
				Free( );

				m_shaderLayouts = std::move( other.m_shaderLayouts );
				m_data = other.m_data;
				m_size = other.m_size;

				other.m_data = nullptr;
				other.m_size = 0;
			}

			return *this;
		}

		~ShaderBindings( )
		{
			Free( );
		}

	private:
		void Allocate( std::size_t size )
		{
			if ( size > 0 )
			{
				assert( m_data == nullptr && m_size == 0 );
				assert( size % sizeof( RefHandle<GraphicsApiResource> ) == 0 );
				m_data = new unsigned char[size];
				m_size = size;

				for ( std::size_t offset = 0; offset < m_size; offset += sizeof( RefHandle<GraphicsApiResource> ) )
				{
					RefHandle<GraphicsApiResource>* handle = reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + offset );
					new ( handle )RefHandle<GraphicsApiResource>( );
				}
			}
		}

		void Free( )
		{
			if ( m_size > 0 )
			{
				for ( std::size_t offset = 0; offset < m_size; offset += sizeof( RefHandle<GraphicsApiResource> ) )
				{
					RefHandle<GraphicsApiResource>* handle = reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + offset );
					handle->~RefHandle<GraphicsApiResource>( );
				}
			}

			m_size = 0;
			delete[] m_data;
			m_data = nullptr;
		}

		std::vector<ShaderBindingLayout> m_shaderLayouts;
		unsigned char* m_data = nullptr;
		std::size_t m_size = 0;
	};
}