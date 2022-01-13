#pragma once

#include "Buffer.h"
#include "DefaultConstantBuffers.h"
#include "ICommandList.h"
#include "ShaderParameterMap.h"

namespace rendercore
{
	template <typename CommandList>
	void BindShaderParameter( CommandList& commandList, const aga::ShaderParameter& parameter, aga::Buffer* buffer )
	{
		switch ( parameter.m_type )
		{
		case aga::ShaderParameterType::ConstantBuffer:
			commandList.BindConstantBuffer( parameter.m_shader, parameter.m_bindPoint, buffer );
			break;
		case aga::ShaderParameterType::SRV:
			commandList.BindSRV( parameter.m_shader, parameter.m_bindPoint, buffer ? buffer->SRV() : nullptr );
			break;
		case aga::ShaderParameterType::UAV:
			commandList.BindUAV( parameter.m_shader, parameter.m_bindPoint, buffer ? buffer->UAV() : nullptr );
			break;
		default:
			assert( false && "Invalid parameter type" );
			break;
		}
	}

	template <typename ValueType>
	void SetShaderValue( const aga::ShaderParameter& parameter, ValueType value )
	{
		assert( parameter.m_bindPoint == 0 );
		assert( parameter.m_type == aga::ShaderParameterType::ConstantBufferValue );
		DefaultConstantBuffers::GetInstance().SetShaderValue( parameter.m_shader, parameter.m_offset, sizeof( ValueType ), &value );
	}
}
