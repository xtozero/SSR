#pragma once

#include "Buffer.h"
#include "GraphicsPipelineState.h"
#include "ICommandList.h"
#include "ShaderBindings.h"
#include "ShaderParameterMap.h"
#include "Texture.h"

namespace rendercore
{
	inline void BindResource( agl::ShaderBindings& bindings, const agl::ShaderParameter& parameter, agl::Buffer* buffer )
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
		case agl::ShaderParameterType::Bindless:
			singleBinding.AddBindless( parameter, buffer ? buffer->SRV() : nullptr );
			break;
		default:
			assert( false && "Invalid parameter type" );
			break;
		}
	}

	inline void BindResource( agl::ShaderBindings& bindings, const agl::ShaderParameter& parameter, agl::Texture* texture, uint32 mipSlice = 0 )
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
			singleBinding.AddUAV( parameter, texture ? texture->UAV( mipSlice ) : nullptr );
			break;
		case agl::ShaderParameterType::Bindless:
			singleBinding.AddBindless( parameter, texture ? texture->SRV() : nullptr );
			break;
		default:
			assert( false && "Invalid parameter type" );
			break;
		}
	}

	inline void BindResource( agl::ShaderBindings& bindings,
		const agl::ShaderParameter& parameter, SamplerState& samplerState )
	{
		agl::SingleShaderBindings singleBinding = bindings.GetSingleShaderBindings( parameter.m_shader );

		if ( singleBinding.GetShaderType() == agl::ShaderType::None )
		{
			return;
		}

		switch ( parameter.m_type )
		{
		case agl::ShaderParameterType::Sampler:
			[[fallthrough]];
		case agl::ShaderParameterType::Bindless:
			singleBinding.AddSampler( parameter, samplerState.Resource() );
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
		case agl::ShaderParameterType::Bindless:
			singleBinding.AddBindless( parameter, nullptr );
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

#define DEFINE_SHADER_PARAM( paramName ) \
public:	\
	const agl::ShaderParameter& paramName##() const \
	{ \
		return m_shaderParam##paramName; \
	} \
private: \
	agl::ShaderParameter m_shaderParam##paramName{ GetShader()->ParameterMap(), #paramName }