#pragma once

#include "PipelineState.h"

#include <d3d12.h>

namespace agl
{
	class D3D12RasterizerState : public RasterizerState
	{
	public:
		D3D12RasterizerState( const RASTERIZER_STATE_TRAIT& trait );
		D3D12RasterizerState( const D3D12RasterizerState& ) = default;
		D3D12RasterizerState( D3D12RasterizerState&& ) = default;
		D3D12RasterizerState& operator=( const D3D12RasterizerState& ) = default;
		D3D12RasterizerState& operator=( D3D12RasterizerState&& ) = default;
		~D3D12RasterizerState() = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		D3D12_RASTERIZER_DESC m_desc;
	};
}
