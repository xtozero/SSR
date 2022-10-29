#pragma once

#include "PipelineState.h"

#include <d3d12.h>

namespace agl
{
	class D3D12SamplerState : public SamplerState
	{
	public:
		D3D12SamplerState( const SAMPLER_STATE_TRAIT& trait );
		D3D12SamplerState( const D3D12SamplerState& ) = default;
		D3D12SamplerState( D3D12SamplerState&& ) = default;
		D3D12SamplerState& operator=( const D3D12SamplerState& ) = default;
		D3D12SamplerState& operator=( D3D12SamplerState&& ) = default;
		~D3D12SamplerState() = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		D3D12_SAMPLER_DESC m_desc;
	};
}
