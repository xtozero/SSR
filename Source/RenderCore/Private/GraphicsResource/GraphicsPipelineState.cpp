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

	BlendState::BlendState( const agl::BlendStateTrait& trait )
	{
		InitResource( trait );
	}

	BlendState::BlendState( agl::BlendState* state ) noexcept
		: m_state( state )
	{
	}

	void BlendState::InitResource( const agl::BlendStateTrait& trait )
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

	DepthStencilState::DepthStencilState( agl::DepthStencilState* state ) noexcept
		: m_state( state )
	{
	}

	void DepthStencilState::InitResource( const agl::DepthStencilStateTrait& trait )
	{
		m_state = agl::DepthStencilState::Create( trait );
		EnqueueRenderTask(
			[state = m_state]()
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

	RasterizerState::RasterizerState( const agl::RasterizerStateTrait& trait )
	{
		InitResource( trait );
	}

	RasterizerState::RasterizerState( agl::RasterizerState* state ) noexcept
		: m_state( state )
	{
	}

	void RasterizerState::InitResource( const agl::RasterizerStateTrait& trait )
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

	SamplerState::SamplerState( const agl::SamplerStateTrait& trait )
	{
		InitResource( trait );
	}

	SamplerState::SamplerState( agl::SamplerState* state ) noexcept
		: m_state( state )
	{
	}

	void SamplerState::InitResource( const agl::SamplerStateTrait& trait )
	{
		m_state = agl::SamplerState::Create( trait );
		EnqueueRenderTask(
			[state = m_state]()
			{
				state->Init();
			} );
	}
}
