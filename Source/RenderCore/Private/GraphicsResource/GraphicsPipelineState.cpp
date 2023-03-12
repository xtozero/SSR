#include "stdafx.h"
#include "GraphicsPipelineState.h"

#include "PipelineState.h"
#include "TaskScheduler.h"

namespace rendercore
{
	agl::BlendState* BlendState::Resource()
	{
		return m_state.Get();
	}

	const agl::BlendState* BlendState::Resource() const
	{
		return m_state.Get();
	}

	BlendState::BlendState( const agl::BLEND_STATE_TRAIT& trait )
	{
		InitResource( trait );
	}

	void BlendState::InitResource( const agl::BLEND_STATE_TRAIT& trait )
	{
		m_state = agl::BlendState::Create( trait );
		EnqueueRenderTask(
			[state = m_state]()
			{
				state->Init();
			} );
	}

	agl::DepthStencilState* DepthStencilState::Resource()
	{
		return m_state.Get();
	}

	const agl::DepthStencilState* DepthStencilState::Resource() const
	{
		return m_state.Get();
	}

	DepthStencilState::DepthStencilState( const agl::DepthStencilStateTrait& trait )
	{
		InitResource( trait );
	}

	void DepthStencilState::InitResource( const agl::DepthStencilStateTrait& trait )
	{
		m_state = agl::DepthStencilState::Create( trait );
		EnqueueRenderTask( [state = m_state]()
			{
				state->Init();
			} );
	}

	agl::RasterizerState* RasterizerState::Resource()
	{
		return m_state.Get();
	}

	const agl::RasterizerState* RasterizerState::Resource() const
	{
		return m_state.Get();
	}

	RasterizerState::RasterizerState( const agl::RASTERIZER_STATE_TRAIT& trait )
	{
		InitResource( trait );
	}

	void RasterizerState::InitResource( const agl::RASTERIZER_STATE_TRAIT& trait )
	{
		m_state = agl::RasterizerState::Create( trait );
		EnqueueRenderTask(
			[state = m_state]()
			{
				state->Init();
			} );
	}

	agl::SamplerState* SamplerState::Resource()
	{
		return m_state.Get();
	}

	const agl::SamplerState* SamplerState::Resource() const
	{
		return m_state.Get();
	}

	SamplerState::SamplerState( const agl::SAMPLER_STATE_TRAIT& trait )
	{
		InitResource( trait );
	}

	void SamplerState::InitResource( const agl::SAMPLER_STATE_TRAIT& trait )
	{
		m_state = agl::SamplerState::Create( trait );
		EnqueueRenderTask(
			[state = m_state]()
			{
				state->Init();
			} );
	}
}
