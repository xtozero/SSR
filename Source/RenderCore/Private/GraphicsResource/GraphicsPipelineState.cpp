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

	BlendState::BlendState( const agl::BlendStateDesc& desc )
	{
		InitResource( desc );
	}

	BlendState::BlendState( agl::BlendState* state ) noexcept
		: m_state( state )
	{
	}

	void BlendState::InitResource( const agl::BlendStateDesc& desc )
	{
		m_state = agl::BlendState::Create( desc );
	}

	agl::DepthStencilState* DepthStencilState::Resource()
	{
		return m_state.Get();
	}

	const agl::DepthStencilState* DepthStencilState::Resource() const
	{
		return m_state.Get();
	}

	DepthStencilState::DepthStencilState( const agl::DepthStencilStateDesc& desc )
	{
		InitResource( desc );
	}

	DepthStencilState::DepthStencilState( agl::DepthStencilState* state ) noexcept
		: m_state( state )
	{
	}

	void DepthStencilState::InitResource( const agl::DepthStencilStateDesc& desc )
	{
		m_state = agl::DepthStencilState::Create( desc );
	}

	agl::RasterizerState* RasterizerState::Resource()
	{
		return m_state.Get();
	}

	const agl::RasterizerState* RasterizerState::Resource() const
	{
		return m_state.Get();
	}

	RasterizerState::RasterizerState( const agl::RasterizerStateDesc& desc )
	{
		InitResource( desc );
	}

	RasterizerState::RasterizerState( agl::RasterizerState* state ) noexcept
		: m_state( state )
	{
	}

	void RasterizerState::InitResource( const agl::RasterizerStateDesc& desc )
	{
		m_state = agl::RasterizerState::Create( desc );
	}

	agl::SamplerState* SamplerState::Resource()
	{
		return m_state.Get();
	}

	const agl::SamplerState* SamplerState::Resource() const
	{
		return m_state.Get();
	}

	SamplerState::SamplerState( const agl::SamplerStateDesc& desc )
	{
		InitResource( desc );
	}

	SamplerState::SamplerState( agl::SamplerState* state ) noexcept
		: m_state( state )
	{
	}

	void SamplerState::InitResource( const agl::SamplerStateDesc& desc )
	{
		m_state = agl::SamplerState::Create( desc );
	}
}
