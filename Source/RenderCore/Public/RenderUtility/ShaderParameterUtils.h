#pragma once

#include "Buffer.h"
#include "ICommandList.h"
#include "ShaderParameterMap.h"
#include "Texture.h"

namespace rendercore
{
	inline void BindResource( agl::ShaderBindings& bindings,
		const agl::ShaderParameter& parameter, agl::Buffer* buffer )
	{
		agl::SingleShaderBindings singleBinding = bindings.GetSingleShaderBindings( parameter.m_shader );

		if ( singleBinding.GetShaderType() == agl::ShaderType::None )
		{
			return;
		}

		switch ( parameter.m_type )
		{
		case agl::ShaderParameterType::ConstantBuffer:
			singleBinding.AddConstantBuffer( parameter, buffer );
			break;
		case agl::ShaderParameterType::SRV:
			singleBinding.AddSRV( parameter, buffer ? buffer->SRV() : nullptr );
			break;
		case agl::ShaderParameterType::UAV:
			singleBinding.AddUAV( parameter, buffer ? buffer->UAV() : nullptr );
			break;
		default:
			assert( false && "Invalid parameter type" );
			break;
		}
	}

	inline void BindResource( agl::ShaderBindings& bindings,
		const agl::ShaderParameter& parameter, agl::Texture* texture )
	{
		agl::SingleShaderBindings singleBinding = bindings.GetSingleShaderBindings( parameter.m_shader );

		if ( singleBinding.GetShaderType() == agl::ShaderType::None )
		{
			return;
		}

		switch ( parameter.m_type )
		{
		case agl::ShaderParameterType::SRV:
			singleBinding.AddSRV( parameter, texture ? texture->SRV() : nullptr );
			break;
		case agl::ShaderParameterType::UAV:
			singleBinding.AddUAV( parameter, texture ? texture->UAV() : nullptr );
			break;
		default:
			assert( false && "Invalid parameter type" );
			break;
		}
	}

	inline void BindResource( agl::ShaderBindings& bindings,
		const agl::ShaderParameter& parameter, agl::SamplerState* samplerState )
	{
		agl::SingleShaderBindings singleBinding = bindings.GetSingleShaderBindings( parameter.m_shader );

		if ( singleBinding.GetShaderType() == agl::ShaderType::None )
		{
			return;
		}

		switch ( parameter.m_type )
		{
		case agl::ShaderParameterType::Sampler:
			singleBinding.AddSampler( parameter, samplerState );
			break;
		default:
			assert( false && "Invalid parameter type" );
			break;
		}
	}

	inline void BindResource( agl::ShaderBindings& bindings,
		const agl::ShaderParameter& parameter, std::nullptr_t )
	{
		agl::SingleShaderBindings singleBinding = bindings.GetSingleShaderBindings( parameter.m_shader );

		if ( singleBinding.GetShaderType() == agl::ShaderType::None )
		{
			return;
		}

		switch ( parameter.m_type )
		{
		case agl::ShaderParameterType::ConstantBuffer:
			singleBinding.AddConstantBuffer( parameter, nullptr );
			break;
		case agl::ShaderParameterType::SRV:
			singleBinding.AddSRV( parameter, nullptr );
			break;
		case agl::ShaderParameterType::UAV:
			singleBinding.AddUAV( parameter, nullptr );
			break;
		case agl::ShaderParameterType::Sampler:
			singleBinding.AddSampler( parameter, nullptr );
			break;
		default:
			assert( false && "Invalid parameter type" );
			break;
		}
	}

	template <typename CommandList, typename ValueType>
	void SetShaderValue( CommandList& commandList, const agl::ShaderParameter& parameter, const ValueType& value )
	{
		assert( parameter.m_bindPoint == 0 );
		assert( parameter.m_type == agl::ShaderParameterType::ConstantBufferValue );
		commandList.SetShaderValue( parameter, &value );
	}
}
