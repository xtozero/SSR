#include "stdafx.h"
#include "Shader.h"

#include "MultiThread/EngineTaskScheduler.h"

Archive& operator<<( Archive& ar, ShaderParameterMap& shaderParamMap )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << shaderParamMap.Size( );
		for ( const auto& shaderParam : shaderParamMap.GetParameterMap() )
		{
			const std::string& variableName = shaderParam.first;
			ShaderParameterType type = shaderParam.second.m_type;
			UINT bindPoint = shaderParam.second.m_bindPoint;
			UINT offfset = shaderParam.second.m_offset;

			ar << variableName << type << bindPoint << offfset;
		}
	}
	else
	{
		std::size_t size;
		ar << size;

		for ( int i = 0; i < size; ++i )
		{
			std::string variableName;
			ShaderParameterType type;
			UINT bindPoint;
			UINT offset;

			ar << variableName << type << bindPoint << offset;

			shaderParamMap.AddParameter( variableName.c_str( ), type, bindPoint, offset );
		}
	}

	return ar;
}

void ShaderBase::Serialize( Archive& ar )
{
	ar << m_byteCode;
	ar << m_parameterMap;
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
