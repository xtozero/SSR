#pragma once

#include "PipelineState.h"

#include <d3d12.h>

namespace agl
{
	class D3D12RasterizerState final : public RasterizerState
	{
	public:
		const D3D12_RASTERIZER_DESC& GetDesc() const
		{
			return m_desc;
		}

		D3D12RasterizerState( const RasterizerStateTrait& trait );
		D3D12RasterizerState( const D3D12RasterizerState& ) = default;
		D3D12RasterizerState( D3D12RasterizerState&& ) = default;
		D3D12RasterizerState& operator=( const D3D12RasterizerState& ) = default;
		D3D12RasterizerState& operator=( D3D12RasterizerState&& ) = default;
		virtual ~D3D12RasterizerState() override = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		D3D12_RASTERIZER_DESC m_desc;
	};
}
