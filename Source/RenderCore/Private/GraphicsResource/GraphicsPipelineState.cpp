#include "stdafx.h"
#include "GraphicsPipelineState.h"

#include "PipelineState.h"
#include "TaskScheduler.h"

namespace rendercore
{
	aga::BlendState* BlendState::Resource()
	{
		return m_state.Get();
	}

	const aga::BlendState* BlendState::Resource() const
	{
		return m_state.Get();
	}

	BlendState::BlendState( const BLEND_STATE_TRAIT& trait )
	{
		InitResource( trait );
	}

	void BlendState::InitResource( const BLEND_STATE_TRAIT& trait )
	{
		m_state = aga::BlendState::Create( trait );
		EnqueueRenderTask(
			[state = m_state]()
			{
				state->Init();
			} );
	}

	aga::DepthStencilState* DepthStencilState::Resource()
	{
		return m_state.Get();
	}

	const aga::DepthStencilState* DepthStencilState::Resource() const
	{
		return m_state.Get();
	}

	DepthStencilState::DepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait )
	{
		InitResource( trait );
	}

	void DepthStencilState::InitResource( const DEPTH_STENCIL_STATE_TRAIT& trait )
	{
		m_state = aga::DepthStencilState::Create( trait );
		EnqueueRenderTask( [state = m_state]()
			{
				state->Init();
			} );
	}

	aga::RasterizerState* RasterizerState::Resource()
	{
		return m_state.Get();
	}

	const aga::RasterizerState* RasterizerState::Resource() const
	{
		return m_state.Get();
	}

	RasterizerState::RasterizerState( const RASTERIZER_STATE_TRAIT& trait )
	{
		InitResource( trait );
	}

	void RasterizerState::InitResource( const RASTERIZER_STATE_TRAIT& trait )
	{
		m_state = aga::RasterizerState::Create( trait );
		EnqueueRenderTask(
			[state = m_state]()
			{
				state->Init();
			} );
	}

	aga::SamplerState* SamplerState::Resource()
	{
		return m_state.Get();
	}

	const aga::SamplerState* SamplerState::Resource() const
	{
		return m_state.Get();
	}

	SamplerState::SamplerState( const SAMPLER_STATE_TRAIT& trait )
	{
		InitResource( trait );
	}

	void SamplerState::InitResource( const SAMPLER_STATE_TRAIT& trait )
	{
		m_state = aga::SamplerState::Create( trait );
		EnqueueRenderTask(
			[state = m_state]()
			{
				state->Init();
			} );
	}
}
