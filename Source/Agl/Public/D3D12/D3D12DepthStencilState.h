#pragma once

#include "PipelineState.h"

#include <d3d12.h>

namespace agl
{
	class D3D12DepthStencilState : public DepthStencilState
	{
	public:
		D3D12DepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait );
		D3D12DepthStencilState( const D3D12DepthStencilState& ) = default;
		D3D12DepthStencilState( D3D12DepthStencilState&& ) = default;
		D3D12DepthStencilState& operator=( const D3D12DepthStencilState& ) = default;
		D3D12DepthStencilState& operator=( D3D12DepthStencilState&& ) = default;
		~D3D12DepthStencilState() = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		D3D12_DEPTH_STENCIL_DESC m_desc;
	};
}
