#include "ComputePipelineState.h"

#include "Shader.h"

namespace rendercore
{
	RefHandle<agl::ComputePipelineState> PrepareComputePipelineState( ComputeShader* computeShader )
	{
		agl::ComputePipelineStateDesc desc{
			.m_computeShader = computeShader->Resource()
		};

		return agl::ComputePipelineState::Create( desc );
	}

	agl::ShaderBindingsInitializer CreateShaderBindingsInitializer( const ComputeShader* computeShader )
	{
		agl::ShaderBindingsInitializer initializer;

		if ( computeShader && computeShader->IsValid() )
		{
			initializer[agl::ShaderType::Compute] = &computeShader->ParameterInfo();
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