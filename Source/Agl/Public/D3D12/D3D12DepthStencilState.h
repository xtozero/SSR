#pragma once

#include "PipelineState.h"

#include <d3d12.h>

namespace agl
{
	class D3D12DepthStencilState : public DepthStencilState
	{
	public:
		const D3D12_DEPTH_STENCIL_DESC& GetDesc() const
		{
			return m_desc;
		}

		D3D12DepthStencilState( const DepthStencilStateTrait& trait );
		D3D12DepthStencilState( const D3D12DepthStencilState& ) = default;
		D3D12DepthStencilState( D3D12DepthStencilState&& ) = default;
		D3D12DepthStencilState& operator=( const D3D12DepthStencilState& ) = default;
		D3D12DepthStencilState& operator=( D3D12DepthStencilState&& ) = default;
		virtual ~D3D12DepthStencilState() override = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		D3D12_DEPTH_STENCIL_DESC m_desc;
	};
}
