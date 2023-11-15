#pragma once

#include "D3D12DescriptorHeapAllocator.h"
#include "PipelineState.h"

#include <d3d12.h>

namespace agl
{
	class D3D12SamplerState final : public SamplerState
	{
	public:
		const D3D12DescriptorHeap& Resource() const;

		D3D12SamplerState( const SamplerStateTrait& trait );
		D3D12SamplerState( const D3D12SamplerState& ) = default;
		D3D12SamplerState( D3D12SamplerState&& ) = default;
		D3D12SamplerState& operator=( const D3D12SamplerState& ) = default;
		D3D12SamplerState& operator=( D3D12SamplerState&& ) = default;
		virtual ~D3D12SamplerState() override = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		D3D12DescriptorHeap m_samplerState;

		D3D12_SAMPLER_DESC m_desc;
	};
}
