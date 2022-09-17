#include "stdafx.h"
#include "PipelineState.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<BlendState> BlendState::Create( const BLEND_STATE_TRAIT& trait )
	{
		return GetInterface<IResourceManager>( )->CreateBlendState( trait );
	}

	RefHandle<DepthStencilState> DepthStencilState::Create( const DEPTH_STENCIL_STATE_TRAIT& trait )
	{
		return GetInterface<IResourceManager>( )->CreateDepthStencilState( trait );
	}

	RefHandle<RasterizerState> RasterizerState::Create( const RASTERIZER_STATE_TRAIT& trait )
	{
		return GetInterface<IResourceManager>( )->CreateRasterizerState( trait );
	}

	RefHandle<SamplerState> SamplerState::Create( const SAMPLER_STATE_TRAIT& trait )
	{
		return GetInterface<IResourceManager>( )->CreateSamplerState( trait );
	}

	RefHandle<VertexLayout> VertexLayout::Create( const VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, uint32 size )
	{
		return GetInterface<IResourceManager>( )->CreateVertexLayout( vs, trait, size );
	}

	RefHandle<PipelineState> PipelineState::Create( const PipelineStateInitializer& initializer )
	{
		return GetInterface<IResourceManager>( )->CreatePipelineState( initializer );
	}
}
