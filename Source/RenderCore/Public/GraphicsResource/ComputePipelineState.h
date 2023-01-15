#pragma once

#include "GraphicsApiResource.h"
#include "PipelineState.h"
#include "ShaderBindings.h"

namespace rendercore
{
	class ComputeShader;

	[[nodiscard]] agl::RefHandle<agl::ComputePipelineState> PrepareComputePipelineState( agl::ComputeShader* cs );

	[[nodiscard]] agl::ShaderBindings CreateShaderBindings( const ComputeShader* computeShader );
}
