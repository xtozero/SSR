#include "PipelineState.h"

#include "HashUtil.h"
#include "InterfaceFactories.h"
#include "IResourceManager.h"

namespace agl
{
	RefHandle<BlendState> BlendState::Create( const BlendStateDesc& desc )
	{
		RefHandle<BlendState> newState = GetInterface<IResourceManager>()->CreateBlendState( desc );
		newState->SetHash( desc.GetHash() );

		EnqueueRenderTask(
			[state = newState]()
			{
				state->Init();
			} );

		return newState;
	}

	RefHandle<DepthStencilState> DepthStencilState::Create( const DepthStencilStateDesc& desc )
	{
		RefHandle<DepthStencilState> newState = GetInterface<IResourceManager>()->CreateDepthStencilState( desc );
		newState->SetHash( desc.GetHash() );

		EnqueueRenderTask(
			[state = newState]()
			{
				state->Init();
			} );

		return newState;
	}

	RefHandle<RasterizerState> RasterizerState::Create( const RasterizerStateDesc& desc )
	{
		RefHandle<RasterizerState> newState = GetInterface<IResourceManager>()->CreateRasterizerState( desc );
		newState->SetHash( desc.GetHash() );

		EnqueueRenderTask(
			[state = newState]()
			{
				state->Init();
			} );

		return newState;
	}

	RefHandle<SamplerState> SamplerState::Create( const SamplerStateDesc& desc )
	{
		RefHandle<SamplerState> newState = GetInterface<IResourceManager>()->CreateSamplerState( desc );
		EnqueueRenderTask(
			[state = newState]()
			{
				state->Init();
			} );

		return newState;
	}

	RefHandle<VertexLayout> VertexLayout::Create( const VertexShader* vs, const VertexLayoutData* layoutData, uint32 size )
	{
		auto layout = GetInterface<IResourceManager>()->CreateVertexLayout( vs, layoutData, size );

		size_t hash = 0;
		for ( uint32 i = 0; i < size; ++i )
		{
			HashCombine( hash, layoutData[i].GetHash() );
		}
		layout->SetHash( hash );

		return layout;
	}

	size_t GraphicsPipelineStateDesc::GetHash() const
	{
		size_t typeHash = typeid( GraphicsPipelineStateDesc ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_vertexShader );
		HashCombine( hash, m_geometryShader );
		HashCombine( hash, m_piexlShader );
		HashCombine( hash, m_meshShader );
		HashCombine( hash, m_amplificationShader );
		HashCombine( hash, m_blendState );
		HashCombine( hash, m_rasterizerState );
		HashCombine( hash, m_depthStencilState );
		HashCombine( hash, m_vertexLayout );
		HashCombine( hash, m_primitiveType );

		return hash;
	}

	size_t ComputePipelineStateDesc::GetHash() const
	{
		static size_t typeHash = typeid( ComputePipelineStateDesc ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_computeShader );

		return hash;
	}

	RefHandle<GraphicsPipelineState> GraphicsPipelineState::Create( const GraphicsPipelineStateDesc& desc )
	{
		auto state = GetInterface<IResourceManager>()->CreatePipelineState( desc );
		state->SetHash( desc.GetHash() );

		return state;
	}

	RefHandle<ComputePipelineState> ComputePipelineState::Create( const ComputePipelineStateDesc& desc )
	{
		auto state = GetInterface<IResourceManager>()->CreatePipelineState( desc );
		state->SetHash( desc.GetHash() );

		return state;
	}
}
