#pragma once

#include "PipelineState.h"
#include "ShaderBindings.h"

namespace rendercore
{
	class ComputeShader;

	[[nodiscard]] RefHandle<agl::ComputePipelineState> PrepareComputePipelineState( ComputeShader* computeShader );

	[[nodiscard]] agl::ShaderBindings CreateShaderBindings( const ComputeShader* computeShader );
}
