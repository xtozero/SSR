#include "ComputePipelineState.h"

#include "Shader.h"

namespace rendercore
{
	agl::RefHandle<agl::ComputePipelineState> PrepareComputePipelineState( agl::ComputeShader* cs )
	{
		agl::ComputePipelineStateInitializer initializer{
			.m_computeShader = cs
		};

		return agl::ComputePipelineState::Create( initializer );
	}

	agl::ShaderBindingsInitializer CreateShaderBindingsInitializer( const ComputeShader* computeShader )
	{
		agl::ShaderBindingsInitializer initializer;

		if ( computeShader && computeShader->IsValid() )
		{
			initializer[agl::ShaderType::CS] = &computeShader->ParameterInfo();
		}

		return initializer;
	}

	agl::ShaderBindings CreateShaderBindings( const ComputeShader* computeShader )
	{
		auto initializer = CreateShaderBindingsInitializer( computeShader );
		agl::ShaderBindings shaderBindings;
		shaderBindings.Initialize( initializer );

		return shaderBindings;
	}
}