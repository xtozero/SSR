#include "PipelineState.h"

#include "HashUtil.h"
#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<BlendState> BlendState::Create( const BlendStateTrait& trait )
	{
		auto state = GetInterface<IResourceManager>()->CreateBlendState( trait );
		state->SetHash( trait.GetHash() );

		return state;
	}

	RefHandle<DepthStencilState> DepthStencilState::Create( const DepthStencilStateTrait& trait )
	{
		auto state = GetInterface<IResourceManager>()->CreateDepthStencilState( trait );
		state->SetHash( trait.GetHash() );

		return state;
	}

	RefHandle<RasterizerState> RasterizerState::Create( const RasterizerStateTrait& trait )
	{
		auto state = GetInterface<IResourceManager>()->CreateRasterizerState( trait );
		state->SetHash( trait.GetHash() );

		return state;
	}

	RefHandle<SamplerState> SamplerState::Create( const SamplerStateTrait& trait )
	{
		return GetInterface<IResourceManager>()->CreateSamplerState( trait );
	}

	RefHandle<VertexLayout> VertexLayout::Create( const VertexShader* vs, const VertexLayoutTrait* trait, uint32 size )
	{
		auto layout = GetInterface<IResourceManager>()->CreateVertexLayout( vs, trait, size );

		size_t hash = 0;
		for ( uint32 i = 0; i < size; ++i )
		{
			HashCombine( hash, trait[i].GetHash() );
		}
		layout->SetHash( hash );

		return layout;
	}

	size_t GraphicsPipelineStateInitializer::GetHash() const
	{
		size_t typeHash = typeid( GraphicsPipelineStateInitializer ).hash_code();
		size_t hash = typeHash;

		if ( m_vertexShader )
		{
			HashCombine( hash, m_vertexShader->GetHash() );
		}

		if ( m_geometryShader )
		{
			HashCombine( hash, m_geometryShader->GetHash() );
		}

		if ( m_piexlShader )
		{
			HashCombine( hash, m_piexlShader->GetHash() );
		}

		if ( m_blendState )
		{
			HashCombine( hash, m_blendState->GetHash() );
		}

		if ( m_rasterizerState )
		{
			HashCombine( hash, m_rasterizerState->GetHash() );
		}

		if ( m_depthStencilState )
		{
			HashCombine( hash, m_depthStencilState->GetHash() );
		}

		if ( m_vertexLayout )
		{
			HashCombine( hash, m_vertexLayout->GetHash() );
		}

		HashCombine( hash, m_primitiveType );

		return hash;
	}

	size_t ComputePipelineStateInitializer::GetHash() const
	{
		static size_t typeHash = typeid( ComputePipelineStateInitializer ).hash_code();
		size_t hash = typeHash;

		if ( m_computeShader )
		{
			HashCombine( hash, m_computeShader->GetHash() );
		}

		return hash;
	}

	RefHandle<GraphicsPipelineState> GraphicsPipelineState::Create( const GraphicsPipelineStateInitializer& initializer )
	{
		auto state = GetInterface<IResourceManager>()->CreatePipelineState( initializer );
		state->SetHash( initializer.GetHash() );

		return state;
	}

	RefHandle<ComputePipelineState> ComputePipelineState::Create( const ComputePipelineStateInitializer& initializer )
	{
		auto state = GetInterface<IResourceManager>()->CreatePipelineState( initializer );
		state->SetHash( initializer.GetHash() );

		return state;
	}
}
