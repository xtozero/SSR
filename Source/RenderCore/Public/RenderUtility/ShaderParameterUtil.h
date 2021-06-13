#pragma once

#include "AgaDelegator.h"
#include "Core/InterfaceFactories.h"
#include "IAga.h"
#include "Shader.h"
#include "ShaderPrameterMap.h"

namespace aga
{
	class Buffer;
}

template <typename ShaderType>
void BindShaderParameter( ShaderType& shader, const aga::ShaderParameter& parameter, aga::Buffer* buffer )
{
	aga::Buffer* buffers[] = { buffer };

	switch ( parameter.m_type )
	{
	case aga::ShaderParameterType::ConstantBuffer:
		GetInterface<aga::IAga>( )->BindConstant( shader.Resource( ), parameter.m_bindPoint, 1, buffers );
		break;
	case aga::ShaderParameterType::SRV:
		GetInterface<aga::IAga>( )->BindShaderInput( shader.Resource( ), parameter.m_bindPoint, 1, buffers );
		break;
	case aga::ShaderParameterType::Sampler:
		break;
	}
}

inline void BindShaderParameter( ComputeShader& shader, const aga::ShaderParameter& parameter, aga::Buffer* buffer )
{
	aga::Buffer* buffers[] = { buffer };

	switch ( parameter.m_type )
	{
	case aga::ShaderParameterType::ConstantBuffer:
		GetInterface<aga::IAga>( )->BindConstant( shader.Resource( ), parameter.m_bindPoint, 1, buffers );
		break;
	case aga::ShaderParameterType::SRV:
		GetInterface<aga::IAga>( )->BindShaderInput( shader.Resource( ), parameter.m_bindPoint, 1, buffers );
		break;
	case aga::ShaderParameterType::UAV:
		GetInterface<aga::IAga>( )->BindShaderOutput( shader.Resource( ), parameter.m_bindPoint, 1, buffers );
		break;
	case aga::ShaderParameterType::Sampler:
		break;
	}
}

template <typename ShaderType, typename ValueType>
void SetShaderValue( ShaderType& shader, const aga::ShaderParameter& parameter, ValueType value )
{
	assert( parameter.m_type == aga::ShaderParameterType::ConstantBufferValue );
	GetAgaDelegator( ).SetShaderValue( shader, parameter, value );
}