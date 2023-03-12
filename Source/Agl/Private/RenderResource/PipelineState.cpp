#include "stdafx.h"
#include "PipelineState.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<BlendState> BlendState::Create( const BlendStateTrait& trait )
	{
		return GetInterface<IResourceManager>()->CreateBlendState( trait );
	}

	RefHandle<DepthStencilState> DepthStencilState::Create( const DepthStencilStateTrait& trait )
	{
		return GetInterface<IResourceManager>()->CreateDepthStencilState( trait );
	}

	RefHandle<RasterizerState> RasterizerState::Create( const RasterizerStateTrait& trait )
	{
		return GetInterface<IResourceManager>()->CreateRasterizerState( trait );
	}

	RefHandle<SamplerState> SamplerState::Create( const SAMPLER_STATE_TRAIT& trait )
	{
		return GetInterface<IResourceManager>()->CreateSamplerState( trait );
	}

	RefHandle<VertexLayout> VertexLayout::Create( const VertexShader* vs, const VertexLayoutTrait* trait, uint32 size )
	{
		return GetInterface<IResourceManager>()->CreateVertexLayout( vs, trait, size );
	}

	RefHandle<GraphicsPipelineState> GraphicsPipelineState::Create( const GraphicsPipelineStateInitializer& initializer )
	{
		return GetInterface<IResourceManager>()->CreatePipelineState( initializer );
	}

	RefHandle<ComputePipelineState> ComputePipelineState::Create( const ComputePipelineStateInitializer& initializer )
	{
		return GetInterface<IResourceManager>()->CreatePipelineState( initializer );
	}
}
