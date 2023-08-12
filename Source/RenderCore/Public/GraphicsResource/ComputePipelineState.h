#pragma once

#include "GraphicsApiResource.h"
#include "PipelineState.h"
#include "ShaderBindings.h"

namespace rendercore
{
	class ComputeShader;

	[[nodiscard]] agl::RefHandle<agl::ComputePipelineState> PrepareComputePipelineState( ComputeShader* computeShader );

	[[nodiscard]] agl::ShaderBindings CreateShaderBindings( const ComputeShader* computeShader );
}
