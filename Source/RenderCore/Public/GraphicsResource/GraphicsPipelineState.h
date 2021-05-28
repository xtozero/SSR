#pragma once

#include "GraphicsApiResource.h"
#include "PipelineState.h"
#include "ShaderBindings.h"
#include "VertexLayout.h"

class BlendState
{
public:
	aga::BlendState* Resource( );
	const aga::BlendState* Resource( ) const;

	explicit BlendState( const BLEND_STATE_TRAIT& trait );

	BlendState( ) = default;
	~BlendState( ) = default;
	BlendState( const BlendState& ) = default;
	BlendState& operator=( const BlendState& ) = default;
	BlendState( BlendState&& ) = default;
	BlendState& operator=( BlendState&& ) = default;

private:
	void InitResource( const BLEND_STATE_TRAIT& trait );

	RefHandle<aga::BlendState> m_state;
};

class DepthStencilState
{
public:
	aga::DepthStencilState* Resource( );
	const aga::DepthStencilState* Resource( ) const;

	explicit DepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait );

	DepthStencilState( ) = default;
	~DepthStencilState( ) = default;
	DepthStencilState( const DepthStencilState& ) = default;
	DepthStencilState& operator=( const DepthStencilState& ) = default;
	DepthStencilState( DepthStencilState&& ) = default;
	DepthStencilState& operator=( DepthStencilState&& ) = default;

private:
	void InitResource( const DEPTH_STENCIL_STATE_TRAIT& trait );

	RefHandle<aga::DepthStencilState> m_state;
};

class RasterizerState
{
public:
	aga::RasterizerState* Resource( );
	const aga::RasterizerState* Resource( ) const;

	explicit RasterizerState( const RASTERIZER_STATE_TRAIT& trait );

	RasterizerState( ) = default;
	~RasterizerState( ) = default;
	RasterizerState( const RasterizerState& ) = default;
	RasterizerState& operator=( const RasterizerState& ) = default;
	RasterizerState( RasterizerState&& ) = default;
	RasterizerState& operator=( RasterizerState&& ) = default;

private:
	void InitResource( const RASTERIZER_STATE_TRAIT& trait );

	RefHandle<aga::RasterizerState> m_state;
};

class SamplerState
{
public:
	aga::SamplerState* Resource( );
	const aga::SamplerState* Resource( ) const;

	explicit SamplerState( const SAMPLER_STATE_TRAIT& trait );

	SamplerState( ) = default;
	~SamplerState( ) = default;
	SamplerState( const SamplerState& ) = default;
	SamplerState& operator=( const SamplerState& ) = default;
	SamplerState( SamplerState&& ) = default;
	SamplerState& operator=( SamplerState&& ) = default;

private:
	void InitResource( const SAMPLER_STATE_TRAIT& trait );

	RefHandle<aga::SamplerState> m_state;
};

struct ShaderStates
{
	VertexLayout m_vertexLayout;
	VertexShader m_vertexShader;
	// Reserved
	// HullShadaer m_hullShader;
	// DomainShader m_domainShader;
	// GeometryShader m_geometryShader;
	PixelShader m_pixelShader;
};

inline ShaderBindingsInitializer CreateShaderBindingsInitializer( const ShaderStates& state )
{
	ShaderBindingsInitializer initializer;
	
	if ( state.m_vertexShader.IsValid( ) )
	{
		initializer[SHADER_TYPE::VS] = &state.m_vertexShader.ParameterInfo( );
	}

	if ( state.m_pixelShader.IsValid( ) )
	{
		initializer[SHADER_TYPE::PS] = &state.m_pixelShader.ParameterInfo( );
	}

	return initializer;
}

struct GraphicsPipelineState
{
	ShaderStates m_shaderState;
	RasterizerState m_rasterizerState;
	DepthStencilState m_depthStencilState;
	BlendState m_blendState;
	RESOURCE_PRIMITIVE m_primitive;

	RefHandle<aga::PipelineState> m_pso;
};