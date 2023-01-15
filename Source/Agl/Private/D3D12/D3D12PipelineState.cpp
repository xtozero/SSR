#include "D3D12PipelineState.h"

#include "D3D12Api.h"

namespace agl
{
	D3D12GraphicsPipelineState::D3D12GraphicsPipelineState( const GraphicsPipelineStateInitializer& initializer )
	{
	}

	void D3D12GraphicsPipelineState::InitResource()
	{
		HRESULT hr = D3D12Device().CreateGraphicsPipelineState( &m_desc, IID_PPV_ARGS( &m_pipelineState ) );
		assert( SUCCEEDED( hr ) && "CreateGraphicsPipelineState failed" );
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
		HRESULT hr = D3D12Device().CreateComputePipelineState( &m_desc, IID_PPV_ARGS( &m_pipelineState ) );
		assert( SUCCEEDED( hr ) && "CreateComputePipelineState failed" );
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
