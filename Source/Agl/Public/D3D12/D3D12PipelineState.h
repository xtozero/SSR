#pragma once

#include "PipelineState.h"

#include <d3d12.h>

namespace agl
{
	class D3D12GraphicsPipelineState : public PipelineState
	{
	public:
		explicit D3D12GraphicsPipelineState( const GraphicsPipelineStateInitializer& initializer );
		D3D12GraphicsPipelineState( const D3D12GraphicsPipelineState& ) = delete;
		D3D12GraphicsPipelineState( D3D12GraphicsPipelineState&& ) = delete;
		D3D12GraphicsPipelineState& operator=( const D3D12GraphicsPipelineState& ) = delete;
		D3D12GraphicsPipelineState& operator=( D3D12GraphicsPipelineState&& ) = delete;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		ID3D12PipelineState* m_pipelineState = nullptr;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc = {};
	};

	class D3D12ComputePipelineState : public PipelineState
	{
	public:
		explicit D3D12ComputePipelineState( const ComputePipelineStateInitializer& initializer );
		D3D12ComputePipelineState( const D3D12ComputePipelineState& ) = delete;
		D3D12ComputePipelineState( D3D12ComputePipelineState&& ) = delete;
		D3D12ComputePipelineState& operator=( const D3D12ComputePipelineState& ) = delete;
		D3D12ComputePipelineState& operator=( D3D12ComputePipelineState&& ) = delete;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		ID3D12PipelineState* m_pipelineState = nullptr;
		D3D12_COMPUTE_PIPELINE_STATE_DESC m_desc = {};
	};
}
