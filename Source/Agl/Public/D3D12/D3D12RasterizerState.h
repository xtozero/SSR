#pragma once

#include "PipelineState.h"

#include <d3d12.h>

namespace agl
{
	class D3D12RasterizerState final : public RasterizerState
	{
	public:
		const D3D12_RASTERIZER_DESC& GetD3DDesc() const
		{
			return m_d3dDesc;
		}

		D3D12RasterizerState( const RasterizerStateDesc& desc );
		D3D12RasterizerState( const D3D12RasterizerState& ) = default;
		D3D12RasterizerState( D3D12RasterizerState&& ) = default;
		D3D12RasterizerState& operator=( const D3D12RasterizerState& ) = default;
		D3D12RasterizerState& operator=( D3D12RasterizerState&& ) = default;
		virtual ~D3D12RasterizerState() override = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		D3D12_RASTERIZER_DESC m_d3dDesc;
	};
}
