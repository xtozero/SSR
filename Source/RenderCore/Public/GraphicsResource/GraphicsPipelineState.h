#pragma once

#include "GraphicsApiResource.h"
#include "PipelineState.h"
#include "ShaderBindings.h"

class DepthStencilState
{
public:
	static DepthStencilState Create( const DEPTH_STENCIL_STATE_TRAIT& trait );

	aga::DepthStencilState* State( )
	{
		return m_state.Get( );
	}

	DepthStencilState( ) = default;

private:
	explicit DepthStencilState( aga::DepthStencilState* state ) : m_state( state ) {}

	RefHandle<aga::DepthStencilState> m_state;
};

class RasterizerState
{
public:
	static RasterizerState Create( const RASTERIZER_STATE_TRAIT& trait );

	aga::RasterizerState* State( )
	{
		return m_state.Get( );
	}

	RasterizerState( ) = default;

private:
	explicit RasterizerState( aga::RasterizerState* state ) : m_state( state ) {}

	RefHandle<aga::RasterizerState> m_state;
};

struct GraphicsPipelineState
{
	ShaderStates m_shaderState;
	RasterizerState m_rasterizerState;
	DepthStencilState m_depthStencilState;
	RE_HANDLE m_blendState;
	RESOURCE_PRIMITIVE m_primitive;
};