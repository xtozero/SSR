#pragma once

#include "GraphicsApiResource.h"
#include "IAga.h"
#include "ShaderPrameterInfo.h"


#include <cassert>
#include <vector>

class ShaderBindingLayout
{
public:
	std::size_t GetDataSize( ) const
	{
		return ( m_parameterInfo.m_constantBuffers.size( ) + m_parameterInfo.m_srvs.size( ) + m_parameterInfo.m_uavs.size( ) + m_parameterInfo.m_samplers.size( ) ) * sizeof( RE_HANDLE );
	}

	std::size_t GetConstantBufferOffset( ) const
	{
		return 0;
	}

	std::size_t GetSRVOffset( ) const
	{
		return m_parameterInfo.m_srvs.size( ) * sizeof( RE_HANDLE );
	}

	std::size_t GetUAVOffset( ) const
	{
		return GetSRVOffset( ) + m_parameterInfo.m_srvs.size( ) * sizeof( RE_HANDLE );
	}

	std::size_t GetSamplerOffset( ) const
	{
		return GetUAVOffset( ) + m_parameterInfo.m_uavs.size( ) * sizeof( RE_HANDLE );
	}

	SHADER_TYPE GetShaderType( ) const
	{
		m_shaderType;
	}

	ShaderBindingLayout( const ShaderParameterInfo& parameterInfo ) : m_parameterInfo( parameterInfo ) {}

protected:
	SHADER_TYPE m_shaderType = SHADER_TYPE::NONE;
	const ShaderParameterInfo& m_parameterInfo;
};

class SingleShaderBindings : public ShaderBindingLayout
{
public:
	void AddConstantBuffer( int slot, const RE_HANDLE& handle )
	{
		assert( IsBufferHandle( handle ) );

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
			GetConstantBufferStart( )[foundSlot] = handle;
		}
	}

	void AddSRV( int slot, const RE_HANDLE& handle )
	{
		assert( IsShaderResourceHandle( handle ) );

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
			GetSRVStart( )[foundSlot] = handle;
		}
	}

	void AddUAV( int slot, const RE_HANDLE& handle )
	{
		assert( IsRandomAccessHandle( handle ) );

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
			GetUAVStart( )[foundSlot] = handle;
		}
	}

	void AddSampler( int slot, const RE_HANDLE& handle )
	{
		assert( IsSamplerStateHandle( handle ) );

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
			GetSamplerStart( )[foundSlot] = handle;
		}
	}

	SingleShaderBindings( const ShaderBindingLayout& bindingLayout, unsigned char* data ) : ShaderBindingLayout( bindingLayout ), m_data( data ) {}

private:
	RE_HANDLE* GetConstantBufferStart( ) const
	{
		return reinterpret_cast<RE_HANDLE*>( m_data + GetConstantBufferOffset( ) );
	}

	RE_HANDLE* GetSRVStart( ) const
	{
		return reinterpret_cast<RE_HANDLE*>( m_data + GetSRVOffset( ) );
	}

	RE_HANDLE* GetUAVStart( ) const
	{
		return reinterpret_cast<RE_HANDLE*>( m_data + GetUAVOffset( ) );
	}

	RE_HANDLE* GetSamplerStart( ) const
	{
		return reinterpret_cast<RE_HANDLE*>( m_data + GetSamplerOffset( ) );
	}

	unsigned char* m_data = nullptr;
};

class ShaderBindings
{
public:
	void Initialize( const IAga& aga, const ShaderStates& shaders )
	{
		std::size_t numShaders = ( shaders.m_vertexShader.IsValid( ) ? 1 : 0 ) +
			( shaders.m_hullShader.IsValid( ) ? 1 : 0 ) +
			( shaders.m_domainShader.IsValid( ) ? 1 : 0 ) +
			( shaders.m_geometryShader.IsValid( ) ? 1 : 0 ) +
			( shaders.m_pixelShader.IsValid( ) ? 1 : 0 );

		m_shaderLayouts.reserve( numShaders );
		std::size_t shaderBindsDataSize = 0;

		if ( shaders.m_vertexShader.IsValid( ) )
		{
			m_shaderLayouts.emplace_back( aga.GetShaderParameterInfo( shaders.m_vertexShader ) );
			shaderBindsDataSize += m_shaderLayouts.back( ).GetDataSize( );
		}

		if ( shaders.m_hullShader.IsValid( ) )
		{
			m_shaderLayouts.emplace_back( aga.GetShaderParameterInfo( shaders.m_hullShader ) );
			shaderBindsDataSize += m_shaderLayouts.back( ).GetDataSize( );
		}

		if ( shaders.m_domainShader.IsValid( ) )
		{
			m_shaderLayouts.emplace_back( aga.GetShaderParameterInfo( shaders.m_domainShader ) );
			shaderBindsDataSize += m_shaderLayouts.back( ).GetDataSize( );
		}

		if ( shaders.m_geometryShader.IsValid( ) )
		{
			m_shaderLayouts.emplace_back( aga.GetShaderParameterInfo( shaders.m_geometryShader ) );
			shaderBindsDataSize += m_shaderLayouts.back( ).GetDataSize( );
		}

		if ( shaders.m_pixelShader.IsValid( ) )
		{
			m_shaderLayouts.emplace_back( aga.GetShaderParameterInfo( shaders.m_pixelShader ) );
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
		static ShaderBindingLayout emptyLayout( emptyParameterInfo );
		return SingleShaderBindings( emptyLayout, nullptr );
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
			assert( size % sizeof( RE_HANDLE ) == 0 );
			m_data = new unsigned char[size];
			m_size = size;

			for ( std::size_t offset = 0; offset < m_size; offset += sizeof( RE_HANDLE ) )
			{
				RE_HANDLE* handle = reinterpret_cast<RE_HANDLE*>( m_data + offset );
				new ( handle )RE_HANDLE( );
			}
		}
	}

	void Free( )
	{
		if ( m_size > 0 )
		{
			for ( std::size_t offset = 0; offset < m_size; offset += sizeof( RE_HANDLE ) )
			{
				RE_HANDLE* handle = reinterpret_cast<RE_HANDLE*>( m_data + offset );
				handle->~RE_HANDLE( );
			}
		}

		m_size = 0;
		m_data = nullptr;
	}

	std::vector<ShaderBindingLayout> m_shaderLayouts;
	unsigned char* m_data = nullptr;
	std::size_t m_size = 0;
};