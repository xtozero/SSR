#include "D3D12PipelineState.h"

namespace agl
{
	D3D12GraphicsPipelineState::D3D12GraphicsPipelineState( const GraphicsPipelineStateInitializer& initializer )
	{
	}

	void D3D12GraphicsPipelineState::InitResource()
	{

	}

	void D3D12GraphicsPipelineState::FreeResource()
	{
		if ( m_pipelineState )
		{
			m_pipelineState->Release();
			m_pipelineState = nullptr;
		}
	}

	D3D12ComputePipelineState::D3D12ComputePipelineState( const ComputePipelineStateInitializer& initializer )
	{
	}

	void D3D12ComputePipelineState::InitResource()
	{
	}

	void D3D12ComputePipelineState::FreeResource()
	{
		if ( m_pipelineState )
		{
			m_pipelineState->Release();
			m_pipelineState = nullptr;
		}
	}
}
