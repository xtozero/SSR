#pragma once

#include "PipelineState.h"

#include <d3d12.h>

namespace agl
{
	class D3D12BlendState : public BlendState
	{
	public:
		D3D12BlendState( const BLEND_STATE_TRAIT& trait );
		D3D12BlendState( const D3D12BlendState& ) = default;
		D3D12BlendState( D3D12BlendState&& ) = default;
		D3D12BlendState& operator=( const D3D12BlendState& ) = default;
		D3D12BlendState& operator=( D3D12BlendState&& ) = default;
		~D3D12BlendState() = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		D3D12_BLEND_DESC m_desc;
	};
}
