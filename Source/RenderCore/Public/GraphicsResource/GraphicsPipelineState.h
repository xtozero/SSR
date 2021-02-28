#pragma once

#include "GraphicsApiResource.h"
#include "PipelineState.h"
#include "ShaderBindings.h"

class DepthStencilState
{
public:
	aga::DepthStencilState* State( )
	{
		return m_state.Get( );
	}

	DepthStencilState( ) = default;
	DepthStencilState( aga::DepthStencilState* state ) : m_state( state ) {}

private:
	RefHandle<aga::DepthStencilState> m_state;
};

struct GraphicsPipelineState
{
	ShaderStates m_shaderState;
	RE_HANDLE m_rasterizerState;
	DepthStencilState m_depthStencilState;
	RE_HANDLE m_blendState;
	RESOURCE_PRIMITIVE m_primitive;
};