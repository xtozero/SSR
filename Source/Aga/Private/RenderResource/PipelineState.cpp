#include "stdafx.h"
#include "PipelineState.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"

namespace aga
{
	RefHandle<BlendState> BlendState::Create( const BLEND_STATE_TRAIT& trait )
	{
		return GetInterface<IAga>( )->CreateBlendState( trait );
	}

	RefHandle<DepthStencilState> DepthStencilState::Create( const DEPTH_STENCIL_STATE_TRAIT& trait )
	{
		return GetInterface<IAga>( )->CreateDepthStencilState( trait );
	}

	RefHandle<RasterizerState> RasterizerState::Create( const RASTERIZER_STATE_TRAIT& trait )
	{
		return GetInterface<IAga>( )->CreateRasterizerState( trait );
	}

	RefHandle<SamplerState> SamplerState::Create( const SAMPLER_STATE_TRAIT& trait )
	{
		return GetInterface<IAga>( )->CreateSamplerState( trait );
	}

	RefHandle<VertexLayout> VertexLayout::Create( const VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, std::size_t size )
	{
		return GetInterface<IAga>( )->CreateVertexLayout( vs, trait, size );
	}

	RefHandle<PipelineState> PipelineState::Create( const PipelineStateInitializer& initializer )
	{
		return GetInterface<IAga>( )->CreatePipelineState( initializer );
	}
}
