#include "stdafx.h"
#include "Shader.h"

#include "MultiThread/EngineTaskScheduler.h"

Archive& operator<<( Archive& ar, ShaderParameter& shaderParam )
{
	ar << shaderParam.m_shader << shaderParam.m_type << shaderParam.m_bindPoint << shaderParam.m_offset << shaderParam.m_sizeInByte;
	return ar;
}

Archive& operator<<( Archive& ar, ShaderParameterMap& shaderParamMap )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << shaderParamMap.Size( );
		for ( auto& shaderParam : shaderParamMap.GetParameterMap() )
		{
			ar << shaderParam.first << shaderParam.second;
		}
	}
	else
	{
		std::size_t size;
		ar << size;

		for ( int i = 0; i < size; ++i )
		{
			std::string variableName;
			ShaderParameter shaderParam;

			ar << variableName << shaderParam;

			shaderParamMap.AddParameter( variableName.c_str( ), shaderParam );
		}
	}

	return ar;
}

Archive& operator<<( Archive& ar, ShaderParameterInfo& shaderParamInfo )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << shaderParamInfo.m_constantBuffers.size( );
		for ( auto& param : shaderParamInfo.m_constantBuffers )
		{
			ar << param;
		}

		ar << shaderParamInfo.m_srvs.size( );
		for ( auto& param : shaderParamInfo.m_srvs )
		{
			ar << param;
		}

		ar << shaderParamInfo.m_uavs.size( );
		for ( auto& param : shaderParamInfo.m_uavs )
		{
			ar << param;
		}

		ar << shaderParamInfo.m_samplers.size( );
		for ( auto& param : shaderParamInfo.m_samplers )
		{
			ar << param;
		}
	}
	else
	{
		std::size_t size;

		ar << size;
		shaderParamInfo.m_constantBuffers.resize( size );
		for ( auto& param : shaderParamInfo.m_constantBuffers )
		{
			ar << param;
		}

		ar << size;
		shaderParamInfo.m_srvs.resize( size );
		for ( auto& param : shaderParamInfo.m_srvs )
		{
			ar << param;
		}

		ar << size;
		shaderParamInfo.m_uavs.resize( size );
		for ( auto& param : shaderParamInfo.m_uavs )
		{
			ar << param;
		}

		ar << size;
		shaderParamInfo.m_samplers.resize( size );
		for ( auto& param : shaderParamInfo.m_samplers )
		{
			ar << param;
		}
	}

	return ar;
}

void ShaderBase::Serialize( Archive& ar )
{
	ar << m_byteCode;
	ar << m_parameterMap;
	ar << m_parameterInfo;
}

REGISTER_ASSET( VertexShader );
void VertexShader::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	ShaderBase::Serialize( ar );
}

aga::VertexShader* VertexShader::Resource( )
{
	return m_shader.Get( );
}

const aga::VertexShader* VertexShader::Resource( ) const
{
	return m_shader.Get( );
}

void VertexShader::PostLoadImpl( )
{
	m_shader = aga::VertexShader::Create( m_byteCode.Data( ), m_byteCode.Size( ) );

	if ( IsInRenderThread( ) )
	{
		m_shader->Init( );
	}
	else
	{
		EnqueueRenderTask( [shader = m_shader]( )
		{
			shader->Init( );
		} );
	}
}

REGISTER_ASSET( PixelShader );
void PixelShader::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	ShaderBase::Serialize( ar );
}

aga::PixelShader* PixelShader::Resource( )
{
	return m_shader.Get( );
}

const aga::PixelShader* PixelShader::Resource( ) const
{
	return m_shader.Get( );
}

void PixelShader::PostLoadImpl( )
{
	m_shader = aga::PixelShader::Create( m_byteCode.Data( ), m_byteCode.Size( ) );

	if ( IsInRenderThread( ) )
	{
		m_shader->Init( );
	}
	else
	{
		EnqueueRenderTask( [shader = m_shader]( )
		{
			shader->Init( );
		} );
	}
}

REGISTER_ASSET( ComputeShader );
void ComputeShader::Serialize( Archive & ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	ShaderBase::Serialize( ar );
}

aga::ComputeShader* ComputeShader::Resource( )
{
	return m_shader.Get( );
}

const aga::ComputeShader* ComputeShader::Resource( ) const
{
	return m_shader.Get( );
}

void ComputeShader::PostLoadImpl( )
{
	m_shader = aga::ComputeShader::Create( m_byteCode.Data( ), m_byteCode.Size( ) );

	if ( IsInRenderThread( ) )
	{
		m_shader->Init( );
	}
	else
	{
		EnqueueRenderTask( [shader = m_shader]( )
		{
			shader->Init( );
		} );
	}
}
