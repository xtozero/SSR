#pragma once

#include "Buffer.h"
#include "Texture.h"
#include "GraphicsApiResource.h"
#include "IAga.h"
#include "PipelineState.h"
#include "Shader.h"
#include "ShaderPrameterInfo.h"
#include "VertexLayout.h"

#include <cassert>
#include <vector>

struct ShaderStates
{
	VertexLayout m_vertexLayout;
	VertexShader m_vertexShader;
	// Reserved
	// HullShadaer m_hullShader;
	// DomainShader m_domainShader;
	// GeometryShader m_geometryShader;
	PixelShader m_pixelShader;
};

class ShaderBindingLayout
{
public:
	std::size_t GetDataSize( ) const
	{
		return ( m_parameterInfo.m_constantBuffers.size( ) + m_parameterInfo.m_srvs.size( ) + m_parameterInfo.m_uavs.size( ) + m_parameterInfo.m_samplers.size( ) ) * sizeof( RefHandle<GraphicsApiResource> );
	}

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

	SHADER_TYPE GetShaderType( ) const
	{
		m_shaderType;
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
	SHADER_TYPE m_shaderType = SHADER_TYPE::NONE;
	ShaderParameterInfo m_parameterInfo;
};

class SingleShaderBindings : public ShaderBindingLayout
{
public:
	void AddConstantBuffer( int slot, aga::Buffer* buffer )
	{
		int foundSlot = -1;

		for ( std::size_t i = 0; i < m_parameterInfo.m_constantBuffers.size(); ++i )
		{
			if ( m_parameterInfo.m_constantBuffers[i].m_bindPoint == slot )
			{
				foundSlot = i;
			}
		}

		if ( foundSlot > 0 )
		{
			GetConstantBufferStart( )[foundSlot] = buffer;
		}
	}

	void AddSRV( int slot, aga::Texture* texture )
	{
		int foundSlot = -1;

		for ( std::size_t i = 0; i < m_parameterInfo.m_srvs.size( ); ++i )
		{
			if ( m_parameterInfo.m_srvs[i].m_bindPoint == slot )
			{
				foundSlot = i;
			}
		}

		if ( foundSlot > 0 )
		{
			GetSRVStart( )[foundSlot] = texture;
		}
	}

	void AddSRV( int slot, aga::Buffer* buffer )
	{
		int foundSlot = -1;

		for ( std::size_t i = 0; i < m_parameterInfo.m_srvs.size( ); ++i )
		{
			if ( m_parameterInfo.m_srvs[i].m_bindPoint == slot )
			{
				foundSlot = i;
			}
		}

		if ( foundSlot > 0 )
		{
			GetSRVStart( )[foundSlot] = buffer;
		}
	}

	void AddUAV( int slot, aga::Texture* texture )
	{
		int foundSlot = -1;

		for ( std::size_t i = 0; i < m_parameterInfo.m_uavs.size( ); ++i )
		{
			if ( m_parameterInfo.m_uavs[i].m_bindPoint == slot )
			{
				foundSlot = i;
			}
		}

		if ( foundSlot > 0 )
		{
			GetUAVStart( )[foundSlot] = texture;
		}
	}

	void AddUAV( int slot, aga::Buffer* buffer )
	{
		int foundSlot = -1;

		for ( std::size_t i = 0; i < m_parameterInfo.m_uavs.size( ); ++i )
		{
			if ( m_parameterInfo.m_uavs[i].m_bindPoint == slot )
			{
				foundSlot = i;
			}
		}

		if ( foundSlot > 0 )
		{
			GetUAVStart( )[foundSlot] = buffer;
		}
	}

	void AddSampler( int slot, aga::SamplerState* sampler )
	{
		int foundSlot = -1;

		for ( std::size_t i = 0; i < m_parameterInfo.m_samplers.size( ); ++i )
		{
			if ( m_parameterInfo.m_samplers[i].m_bindPoint == slot )
			{
				foundSlot = i;
			}
		}

		if ( foundSlot > 0 )
		{
			GetSamplerStart( )[foundSlot] = sampler;
		}
	}

	SingleShaderBindings( const ShaderBindingLayout& bindingLayout, unsigned char* data ) : ShaderBindingLayout( bindingLayout ), m_data( data ) {}

private:
	RefHandle<aga::Buffer>* GetConstantBufferStart( ) const
	{
		return reinterpret_cast<RefHandle<aga::Buffer>*>( m_data + GetConstantBufferOffset( ) );
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

	unsigned char* m_data = nullptr;
};

class ShaderBindings
{
public:
	void Initialize( const ShaderStates& shaders )
	{
		std::size_t numShaders = ( shaders.m_vertexShader.IsValid( ) ? 1 : 0 ) +
			//( shaders.m_hullShader.IsValid( ) ? 1 : 0 ) +
			//( shaders.m_domainShader.IsValid( ) ? 1 : 0 ) +
			//( shaders.m_geometryShader.IsValid( ) ? 1 : 0 ) +
			( shaders.m_pixelShader.IsValid( ) ? 1 : 0 );

		m_shaderLayouts.reserve( numShaders );
		std::size_t shaderBindsDataSize = 0;

		if ( shaders.m_vertexShader.IsValid( ) )
		{
			m_shaderLayouts.emplace_back( SHADER_TYPE::VS, shaders.m_vertexShader.ParameterInfo( ) );
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

		if ( shaders.m_pixelShader.IsValid( ) )
		{
			m_shaderLayouts.emplace_back( SHADER_TYPE::PS, shaders.m_vertexShader.ParameterInfo( ) );
			shaderBindsDataSize += m_shaderLayouts.back( ).GetDataSize( );
		}

		Allocate( shaderBindsDataSize );
	}

	SingleShaderBindings GetSingleShaderBindings( SHADER_TYPE shaderType )
	{
		for ( auto iter = m_shaderLayouts.begin( ); iter != m_shaderLayouts.end( ); ++iter )
		{
			std::size_t dataOffset = 0;

			if ( iter->GetShaderType( ) == shaderType )
			{
				return SingleShaderBindings( *iter, m_data + dataOffset );
			}

			dataOffset += iter->GetDataSize();
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
		m_data = nullptr;
	}

	std::vector<ShaderBindingLayout> m_shaderLayouts;
	unsigned char* m_data = nullptr;
	std::size_t m_size = 0;
};