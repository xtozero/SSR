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
void BindShaderParameter( ShaderType& shader, const ShaderParameter& parameter, aga::Buffer* buffer )
{
	aga::Buffer* buffers[] = { buffer };

	switch ( parameter.m_type )
	{
	case ShaderParameterType::ConstantBuffer:
		GetInterface<IAga>( )->BindConstant( shader, parameter.m_bindPoint, 1, buffers );
		break;
	case ShaderParameterType::SRV:
		GetInterface<IAga>( )->BindShaderInput( shader, parameter.m_bindPoint, 1, buffers );
		break;
	case ShaderParameterType::Sampler:
		break;
	}
}

inline void BindShaderParameter( ComputeShader& shader, const ShaderParameter& parameter, aga::Buffer* buffer )
{
	aga::Buffer* buffers[] = { buffer };

	switch ( parameter.m_type )
	{
	case ShaderParameterType::ConstantBuffer:
		GetInterface<IAga>( )->BindConstant( shader, parameter.m_bindPoint, 1, buffers );
		break;
	case ShaderParameterType::SRV:
		GetInterface<IAga>( )->BindShaderInput( shader, parameter.m_bindPoint, 1, buffers );
		break;
	case ShaderParameterType::UAV:
		GetInterface<IAga>( )->BindShaderOutput( shader, parameter.m_bindPoint, 1, buffers );
		break;
	case ShaderParameterType::Sampler:
		break;
	}
}

template <typename ShaderType, typename ValueType>
void SetShaderValue( ShaderType& shader, const ShaderParameter& parameter, ValueType value )
{
	assert( parameter.m_type == ShaderParameterType::ConstantBufferValue );
	GetAgaDelegator( ).SetShaderValue( shader, parameter, value );
}