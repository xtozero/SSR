#include "stdafx.h"
#include "Shader.h"

#include "ArchiveUtility.h"
#include "TaskScheduler.h"

Archive& operator<<( Archive& ar, aga::ShaderParameter& shaderParam )
{
	ar << shaderParam.m_shader << shaderParam.m_type << shaderParam.m_bindPoint << shaderParam.m_offset << shaderParam.m_sizeInByte;
	return ar;
}

Archive& operator<<( Archive& ar, aga::ShaderParameterMap& shaderParamMap )
{
	ar << shaderParamMap.GetParameterMap( );
	return ar;
}

Archive& operator<<( Archive& ar, aga::ShaderParameterInfo& shaderParamInfo )
{
	ar << shaderParamInfo.m_constantBuffers;
	ar << shaderParamInfo.m_srvs;
	ar << shaderParamInfo.m_uavs;
	ar << shaderParamInfo.m_samplers;
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
	EnqueueRenderTask( [shader = m_shader]( )
	{
		shader->Init( );
	} );
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
	EnqueueRenderTask( [shader = m_shader]( )
	{
		shader->Init( );
	} );
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
	EnqueueRenderTask( [shader = m_shader]( )
	{
		shader->Init( );
	} );
}
