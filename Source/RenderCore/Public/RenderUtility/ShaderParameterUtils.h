#pragma once

#include "Buffer.h"
#include "DefaultConstantBuffers.h"
#include "ICommandList.h"
#include "ShaderParameterMap.h"
#include "Texture.h"

namespace rendercore
{
	template <typename CommandList>
	void BindShaderParameter( CommandList& commandList, const agl::ShaderParameter& parameter, agl::Buffer* buffer )
	{
		switch ( parameter.m_type )
		{
		case agl::ShaderParameterType::ConstantBuffer:
			commandList.BindConstantBuffer( parameter.m_shader, parameter.m_bindPoint, buffer );
			break;
		case agl::ShaderParameterType::SRV:
			commandList.BindSRV( parameter.m_shader, parameter.m_bindPoint, buffer ? buffer->SRV() : nullptr );
			break;
		case agl::ShaderParameterType::UAV:
			commandList.BindUAV( parameter.m_shader, parameter.m_bindPoint, buffer ? buffer->UAV() : nullptr );
			break;
		default:
			assert( false && "Invalid parameter type" );
			break;
		}
	}

	template <typename CommandList>
	void BindShaderParameter( CommandList& commandList, const agl::ShaderParameter& parameter, agl::Texture* texture )
	{
		switch ( parameter.m_type )
		{
		case agl::ShaderParameterType::SRV:
			commandList.BindSRV( parameter.m_shader, parameter.m_bindPoint, texture ? texture->SRV() : nullptr );
			break;
		case agl::ShaderParameterType::UAV:
			commandList.BindUAV( parameter.m_shader, parameter.m_bindPoint, texture ? texture->UAV() : nullptr );
			break;
		default:
			assert( false && "Invalid parameter type" );
			break;
		}
	}

	template <typename CommandList>
	void BindShaderParameter( CommandList& commandList, const agl::ShaderParameter& parameter, agl::SamplerState* samplerState )
	{
		switch ( parameter.m_type )
		{
		case agl::ShaderParameterType::Sampler:
			commandList.BindSampler( parameter.m_shader, parameter.m_bindPoint, samplerState );
			break;
		default:
			assert( false && "Invalid parameter type" );
			break;
		}
	}

	template <typename CommandList>
	void BindShaderParameter( CommandList& commandList, const agl::ShaderParameter& parameter, std::nullptr_t )
	{
		switch ( parameter.m_type )
		{
		case agl::ShaderParameterType::ConstantBuffer:
			commandList.BindConstantBuffer( parameter.m_shader, parameter.m_bindPoint, nullptr );
			break;
		case agl::ShaderParameterType::SRV:
			commandList.BindSRV( parameter.m_shader, parameter.m_bindPoint, nullptr );
			break;
		case agl::ShaderParameterType::UAV:
			commandList.BindUAV( parameter.m_shader, parameter.m_bindPoint, nullptr );
			break;
		case agl::ShaderParameterType::Sampler:
			commandList.BindSampler( parameter.m_shader, parameter.m_bindPoint, nullptr );
			break;
		}
	}

	template <typename ValueType>
	void SetShaderValue( const agl::ShaderParameter& parameter, ValueType value )
	{
		assert( parameter.m_bindPoint == 0 );
		assert( parameter.m_type == agl::ShaderParameterType::ConstantBufferValue );
		DefaultConstantBuffers::GetInstance().SetShaderValue( parameter.m_shader, parameter.m_offset, sizeof( ValueType ), &value );
	}
}
