#pragma once

#include "GraphicsApiResource.h"
#include "PipelineState.h"
#include "ShaderBindings.h"

class BlendState
{
public:
	static BlendState Create( const BLEND_STATE_TRAIT& trait );

	aga::BlendState* State( )
	{
		return m_state.Get( );
	}

	BlendState( ) = default;

private:
	explicit BlendState( aga::BlendState* state ) : m_state( state ) {}

	RefHandle<aga::BlendState> m_state;
};

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

class SamplerState
{
public:
	static SamplerState Create( const SAMPLER_STATE_TRAIT& trait );

	aga::SamplerState* State( )
	{
		return m_state.Get( );
	}

	SamplerState( ) = default;

private:
	explicit SamplerState( aga::SamplerState* state ) : m_state( state ) {}

	RefHandle<aga::SamplerState> m_state;
};

struct GraphicsPipelineState
{
	ShaderStates m_shaderState;
	RasterizerState m_rasterizerState;
	DepthStencilState m_depthStencilState;
	BlendState m_blendState;
	RESOURCE_PRIMITIVE m_primitive;
};